---
services: microsoft-information-protection
platforms: cpp
author: tommoser
level: 300
client: Desktop
service: Microsoft Information Protection
---

# MipSdk-FileApi-Cpp-Sample-Basic

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
- Python 2.7 installed and in the system path

### Sample Setup

> **Project folder** refers to the **MipSdk-FileApi-Cpp-Sample-Basic\MipSdk-FileApi-Cpp-Sample-Basic** directory in the folder where you cloned the repository.

1. Download MIP SDK Binaries from https://aka.ms/mipsdkbinaries and extract to a folder.
2. From a command prompt, run: **git clone https://github.com/Azure-Samples/MipSdk-FileApi-Cpp-Sample-Basic**
3. In the folder where the ZIP was extracted, locate **mip_sdk_file_win32_version.zip** and extract the contents to a folder.
4. Open the folder where the ZIP file was extracted in Windows Explorer.
5. Copy the **include** folder in to the project folder.
6. Navigate to **bins\debug\amd64** in the extracted File API ZIP.
7. Copy *.lib to the project folder.
8. Create a folder called **sdkbins** in the project folder.
9. Copy *.dll from the **bins\debug\amd64** folder in to the **sdkbins** folder.
10. Launch the project by double-clicking **MipSdk-FileApi-Cpp-Sample-Basic.sln**
11. When the project starts, set the project type to **x64**
12. In Visual Studio, click the **Build** menu, then click **Build**. The application should compile at this point, but will crash if run.
13. Continue to the steps below to configure the Azure AD App Registration and update the sample code.

### Create an Azure AD App Registration

Authentication against the Azure AD tenant requires creating a native application registration. The client ID created in this step is used in a later step to generate an OAuth2 token.

> Skip this step if you've already created a registration for previous sample. You may continue to use that client ID.

1. Go to https://portal.azure.com and log in as a global admin.
> Your tenant may permit standard users to register applications. If you aren't a global admin, you can attempt these steps, but may need to work with a tenant administrator to have an application registered or be granted access to register applications.
2. Click Azure Active Directory, then **App Registrations** in the menu blade.
3. Click **View all applications**
4. Click **New Applications Registration**
5. For name, enter **MipSdk-Sample-Apps**
6. Set **Application Type** to **Native**
7. For Redirect URI, enter **mipsdk-auth-sample://authorize**   
  > Note: This can be anything you'd like, but should be unique in the tenant.
8. Click **Create**

The **Registered app** blade should now be displayed.

1. Click **Settings**
2. Click **Required Permissions**
3. Click **Add**
4. Click **Select an API**
5. Select **Microsoft Rights Management Services** and click **Select**
6. Under **Select Permissions** select **Create and access protected content for users**
7. Click **Select** then **Done**
8. Click **Add**
9. Click **Select an API**
10. In the search box, type **Microsoft Information Protection Sync Service** then select the service and click **Select**
11. Under **Select Permissions** select **Read all unified policies a user has access to.**
12. Click **Select** then **Done**
13. In the **Required Permissions** blade, click **Grant Permissions** and confirm.

### Update Client ID, Username, and Password

1. Open up **main.cpp**.
2. Find line 57 and replace **YOUR CLIENT HERE** with the client ID copied from the AAD App Registration.
3. Find line 64 and enter your test username and password.

> Hard coding a username and password isn't recommended. For the scope of this sample, it's an easier way to abstract auth.

## Run the Sample

Press F5 to run the sample. The console application will start and after a brief moment displays the labels available for the user.

- Copy a label ID to the clipboard.
- Paste the label in to the input prompt.
- Next, the app asks for a path to a file. Enter the path to an Office document or PDF file.
- Finally, the app will display the name of the applied label.
- Attempt to open the file in a viewer that supports labeling or protection (Office or Adobe Reader)

## Resources

- [Microsoft Information Protection Docs](https://aka.ms/mipsdkdocs)
