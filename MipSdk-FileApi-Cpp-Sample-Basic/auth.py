#
# Copyright (c) Microsoft Corporation.
# All rights reserved.
#
# This code is licensed under the MIT License.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# make sure to run pip install adal, first. 
# for non-windows setup, review https://github.com/AzureAD/azure-activedirectory-library-for-python/wiki/ADAL-basics

import getopt
import sys
import json
import re
from msal import PublicClientApplication

def printUsage():
  print('auth.py -u <username> -p <password> -a <authority> -r <resource> -c <clientId>')

def main(argv):
  try:
    options, args = getopt.getopt(argv, 'hu:p:a:r:c:')
  except getopt.GetoptError:
    printUsage()
    sys.exit(-1)

  username = ''
  password = ''
  authority = ''
  resource = ''

  clientId = ''
    
  for option, arg in options:
    if option == '-h':
      printUsage()
      sys.exit()
    elif option == '-u':
      username = arg
    elif option == '-p':
      password = arg
    elif option == '-a':
      authority = arg
    elif option == '-r':
      resource = arg
    elif option == '-c':
      clientId = arg

  if username == '' or password == '' or authority == '' or resource == '' or clientId == '':
    printUsage()
    sys.exit(-1)

  # ONLY FOR DEMO PURPOSES AND MSAL FOR PYTHON
  # This shouldn't be required when using proper auth flows in production.  
  if authority.find('common') > 1:
    authority = authority.split('/common')[0] + "/organizations"
   
  app = PublicClientApplication(client_id=clientId, authority=authority)  
  
  result = None  

  if resource.endswith('/'):
    resource += ".default"    
  else:
    resource += "/.default"
  
  # *DO NOT* use username/password authentication in production system.
  # Instead, consider auth code flow and using a browser to fetch the token.
  result = app.acquire_token_by_username_password(username=username, password=password, scopes=[resource])  
  print(result['access_token'])

if __name__ == '__main__':  
  main(sys.argv[1:])
