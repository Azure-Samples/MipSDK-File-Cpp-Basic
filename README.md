---
page_type: sample
languages:
- cpp
- python
products:
- m365
- office-365
description: "This sample application illustrates using the MIP File API to list labels, apply a label, then read the label. All SDK actions are implemented in action.cpp."
urlFragment: MipSDK-File-Cpp-Basic
---

# MipSDK-File-Cpp-Basic

This sample illustrates basic SDK functionality where it:

- Obtains the list of labels for the user
- Prompts to input one of the label IDs
- Prompts for a file path of a file to label
- Applies the label
- Reads the label from the document and displays metadata

## Summary

This sample application illustrates using the MIP File API to list labels, apply a label, then read the label. All SDK actions are implemented in **action.cpp**.

## Getting Started

### Prerequisites

- Visual Studio 2015 or later with Visual C++ development features installed
- Python 3+ installed and in the system path

### Sample Setup

In Visual Studio 2017:

1. Right-click the project and select **Manage NuGet Packages**
2. On the **Browse** tab, search for *Microsoft.InformationProtection.File*
3. Select the package and click **Install**

### Create an Azure AD App Registration

Authentication against the Azure AD tenant requires creating a native application registration. The client ID created in this step is used in a later step to generate an OAuth2 token.

> Skip this step if you've already created a registration for previous sample. You may continue to use that client ID.

1. Go to https://portal.azure.com and log in as a global admin.
   > Your tenant may permit standard users to register applications. If you aren't a global admin, you can attempt these steps, but may need to work with a tenant administrator to have an application registered or be granted access to register applications.
2. Select Azure Active Directory, then **App Registrations** on the left side menu.
3. Select **New registration**
4. For name, enter **MipSdk-Sample-Apps**
5. Under **Supported account types** set **Accounts in this organizational directory only**
   > Optionally, set this to **Accounts in any organizational directory**.
6. Select **Register**

The **Application registration** screen should now be displaying your new application.

### Add API Permissions 

1. Select **API Permissions**
2. Select **Add a permission**
3. Select **Azure Rights Management Services**
4. Select **Delegated permissions**
5. Check **user_impersonation** and select **Add permissions** at the bottom of the screen.
6. Select **Add a permission**
7. Select **APIs my organization uses**
8. In the search box, type **Microsoft Information Protection Sync Service** then select the service.
9. Select **Delegated permissions**
10. Check **UnifiedPolicy.User.Read** then select **Add permissions**
11. In the **API permissions** menu, select **Grant admin consent for <TENANT NAME>** and confirm.

### Set Redirect URI

1. Select **Authentication**.
2. Select **Add a platform**.
3. Select **Mobile and desktop applications**
4. Select the default native client redirect URI, which should look similar to **https://login.microsoftonline.com/common/oauth2/nativeclient**.
5. Under **Advanced settings** set **Treat as a public client** to **yes**.
   > This is required only for the MIP SDK sample apps using MSAL for Python.
6. Select **configure** and be sure to save and changes if required. 

### Update Client ID, Username, and Password

1. Open up **main.cpp**.
2. Replace **YOUR CLIENT ID HERE** with the client ID copied from the AAD App Registration.
3. Find the tokens for **YOUR USERNAME HERE** and **YOUR PASSWORD HERE** and insert test user credentials. 

> DO NOT hard code a production username and password.

## Run the Sample

Press F5 to run the sample. The console application will start and after a brief moment displays the labels available for the user.

- Copy a label ID to the clipboard.
- Paste the label in to the input prompt.
- Next, the app asks for a path to a file. Enter the path to an Office document or PDF file.
- Finally, the app will display the name of the applied label.
- Attempt to open the file in a viewer that supports labeling or protection (Office or Adobe Reader)

## Resources

- [Microsoft Information Protection Docs](https://aka.ms/mipsdkdocs)
