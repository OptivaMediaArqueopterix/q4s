call winhttpjs.bat "http://212.170.54.119:8280/policyserver-webcontroller/login"  -method POST  -saveTo token.txt -body-file auth.json
echo DONE
