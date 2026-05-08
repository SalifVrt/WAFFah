import requests
import sys

#waffah running on this URL
PROXY_URL = "http://localhost:8888"

#test cases definition: (test name, URL path, expected http status code)
test_cases = [
    ("Legitimate: Home page access", "/", 200),
    ("Legitimate: Standard search query", "/?search=books", 200),
    ("XSS: Basic script tag injection", "/?q=<script>alert(1)</script>", 403),
    ("XSS: Encoded script tag", "/?q=%3Cscript%3Ealert(1)%3C/script%3E", 403),
    ("SQLi: Classic logic bypass", "/login?user=admin' OR 1=1--", 403),
    ("LFI: Simple path traversal", "/../../etc/passwd", 403),
    ("LFI: Deep path traversal", "/static/../../../../var/log/syslog", 403),
    ("Forbidden: Accessing admin area", "/admin", 403),
    ("Forbidden: Accessing sensitive config", "/.env", 403),
]

def run_tests():
    print(f"Starting WAF functional test suite on {PROXY_URL}...")
    print("-" * 60)
    
    passed_count = 0
    session = requests.Session()
    
    # run standard security tests
    for name, path, expected_code in test_cases:
        try:
            target_url = PROXY_URL + path
            
            #using prepared requests to bypass automatic URL normalization
            req = requests.Request('GET', target_url)
            prepped = session.prepare_request(req)
            prepped.url = target_url 
            
            response = session.send(prepped, timeout=5)
            
            if response.status_code == expected_code:
                print(f"[PASSED] {name}")
                passed_count += 1
            else:
                print(f"[FAILED] {name}")
                print(f"    -> Expected: {expected_code}")
                print(f"    -> Received: {response.status_code}")
                
        except requests.exceptions.ConnectionError:
            print(f"[ERROR] {name}")
            print(f"    -> Could not connect to proxy. Ensure it is running.")
        except Exception as e:
            print(f"[ERROR] {name}")
            print(f"    -> An unexpected error occurred: {e}")
            
    #run Dynamic Buffer Stress Test
    print("-" * 60)
    print("Running Stress Test: Dynamic Buffer Reallocation...")
    
    stress_test_name = "Stress: Large HTTP Header (> 4096 bytes)"
    try:
        #we generate a 6000 character string to force the proxy to use realloc()
        large_payload = "A" * 6000
        headers = {"X-Large-Payload": large_payload}
        
        response = session.get(PROXY_URL + "/", headers=headers, timeout=5)
        
        #if we get a 200 OK, it means the proxy successfully reallocated 
        # memory, read the whole request, and forwarded it without crashing.
        if response.status_code == 200:
            print(f"[PASSED] {stress_test_name}")
            passed_count += 1
        else:
            print(f"[FAILED] {stress_test_name} (Got Status: {response.status_code})")
    except Exception as e:
         print(f"[ERROR] {stress_test_name}")
         print(f"    -> Buffer crash or connection lost: {e}")

    #final Results
    total_tests = len(test_cases) + 1
    print("-" * 60)
    print(f"FINAL RESULT: {passed_count}/{total_tests} tests passed.")
    
    if passed_count != total_tests:
        sys.exit(1)

if __name__ == "__main__":
    run_tests()