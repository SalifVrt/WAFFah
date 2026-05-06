import requests
import sys

# proxy running on this URL
PROXY_URL = "http://localhost:8888"

# Test cases definition: (Test Name, URL Path, Expected HTTP Status Code)
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
    print(f"starting WAF functional test suite on {PROXY_URL}...")
    print("-" * 60)
    
    passed_count = 0
    
    for name, path, expected_code in test_cases:
        try:
            url = PROXY_URL + path
            # We set a timeout to ensure the test script doesn't hang
            response = requests.get(url, timeout=5)
            
            if response.status_code == expected_code:
                print(f"✅ [PASSED] {name}")
                passed_count += 1
            else:
                print(f"❌ [FAILED] {name}")
                print(f"    -> Expected: {expected_code}")
                print(f"    -> Received: {response.status_code}")
                
        except requests.exceptions.ConnectionError:
            print(f"[ERROR] {name}")
            print(f"    -> Could not connect to proxy. Ensure it is running on {PROXY_URL}.")
        except Exception as e:
            print(f"[ERROR] {name}")
            print(f"    -> An unexpected error occurred: {e}")
            
    print("-" * 60)
    print(f"FINAL RESULT: {passed_count}/{len(test_cases)} tests passed.")
    
    # Return non-zero exit code if any test failed (useful for CI pipeline)
    if passed_count != len(test_cases):
        sys.exit(1)

if __name__ == "__main__":
    run_tests()