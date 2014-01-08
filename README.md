vbrute
==================

Virtual hosts brute forcer. Specify file containing domains and file containing
IPs and vbrute will attempt to connect to IP with specific domain.

Usage:

vbrute domainsFile ipsFile

Sample output:

-----------------------------------------------------------
Domain                  IP              Code      Length
-----------------------------------------------------------
google.com              212.58.253.67   301       229       
cnn.com                 212.58.253.67   301       229       
test.com                212.58.253.67   301       229       
-----------------------------------------------------------
Domain                  IP              Code      Length
-----------------------------------------------------------
google.com              173.194.41.88   301       219       
cnn.com                 173.194.41.88   302       219       
test.com                173.194.41.88   302       219
