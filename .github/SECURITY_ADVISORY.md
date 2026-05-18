# Security Advisory

Sentinel detected 1 security finding in your GitHub Actions workflows that require manual review.

## shell-injection-expr

### ci.yml (line 41)

**Severity:** critical
**Issue:** Attacker-controllable expression ${{ github.head_ref }} in run: block — shell injection risk
**Fix:** Move to env: block and reference as $ENV_VAR in the shell

