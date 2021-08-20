/**
*
* Copyright (c) Microsoft Corporation.
* All rights reserved.
*
* This code is licensed under the MIT License.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
*/

#include "action.h"


#include "mip/common_types.h"
#include "mip/file/file_profile.h"
#include "mip/file/file_engine.h"
#include "mip/file/file_handler.h"
#include "mip/file/labeling_options.h"


#include "auth_delegate_impl.h"
#include "consent_delegate_impl.h"
#include "file_profile_observer_impl.h"
#include "file_handler_observer_impl.h"
#include "utils.h"

#include <iostream>
#include <future>

using std::cout;
using std::endl;

using mip::FileProfile;
using mip::FileEngine;
using mip::FileHandler;

namespace sample {
	namespace file {

		// Constructor accepts mip::ApplicationInfo object and uses it to initialize AuthDelegateImpl.
		// Specifically, AuthDelegateInfo uses mAppInfo.ApplicationId for AAD client_id value.		
		Action::Action(const mip::ApplicationInfo appInfo,
			const std::string& username,
			const std::string& password,
			const bool generateAuditEvents)
			: mAppInfo(appInfo),
			mUsername(username),
			mPassword(password),
			mGenerateAuditEvents(generateAuditEvents) {
			mAuthDelegate = std::make_shared<sample::auth::AuthDelegateImpl>(mAppInfo, mUsername, mPassword);				
		}

		// Implement destructor to null MIPContext, profile and engine references.
		Action::~Action()
		{			
			mEngine = nullptr;
			mProfile = nullptr;
			mMipContext->ShutDown();
			mMipContext = nullptr;
		}

		// Method illustrates how to create a new mip::FileProfile using promise/future
		// Result is stored in private mProfile variable and referenced throughout lifetime of Action.
		void sample::file::Action::AddNewFileProfile()
		{			
			// Initialize MipConfiguration.
			std::shared_ptr<mip::MipConfiguration> mipConfiguration = std::make_shared<mip::MipConfiguration>(mAppInfo,
																												"file_sample",
																												mip::LogLevel::Trace,
																												false);

			// Initialize MipContext. MipContext can be set to null at shutdown and will automatically release all resources.
			mMipContext = mip::MipContext::Create(mipConfiguration);
			
			// Initialize the FileProfile::Settings Object.  
			// Accepts MipContext, AuthDelegate, new ConsentDelegate, new FileProfile::Observer object as last parameters.
			FileProfile::Settings profileSettings(mMipContext,
				mip::CacheStorageType::OnDiskEncrypted, 			
				std::make_shared<sample::consent::ConsentDelegateImpl>(),
				std::make_shared<FileProfileObserver>());
			
			// Create promise and future for mip::FileProfile object.
			std::shared_ptr<std::promise<std::shared_ptr<mip::FileProfile>>> profilePromise = std::make_shared<std::promise<std::shared_ptr<FileProfile>>>();
			std::future<std::shared_ptr<mip::FileProfile>> profileFuture = profilePromise->get_future();

			// Call static function LoadAsync providing the settings and promise. This will make the profile available to use.
			FileProfile::LoadAsync(profileSettings, profilePromise);

			// Get the future value and store in mProfile. mProfile is used throughout Action for profile operations.
			mProfile = profileFuture.get();
		}

		// Action::AddNewFileEngine adds an engine for a specific user. 		
		void Action::AddNewFileEngine()
		{
			// If mProfile hasn't been set, use AddNewFileProfile() to set it.
			if (!mProfile)
			{
				AddNewFileProfile();
			}

			// FileEngine requires a FileEngine::Settings object. The first parameter is the user identity or engine ID. 
			FileEngine::Settings engineSettings(mip::Identity(mUsername), 
													mAuthDelegate, 
													"", 
													"en-US", 
													false);

			// Create promise and future for mip::FileEngine object
			std::shared_ptr<std::promise<std::shared_ptr<mip::FileEngine>>> enginePromise = std::make_shared<std::promise<std::shared_ptr<FileEngine>>>();
			std::future<std::shared_ptr<mip::FileEngine>> engineFuture = enginePromise->get_future();

			// Engines are added to profiles. Call AddEngineAsync on mProfile, providing settings and promise
			// then get the future value and set in mEngine. mEngine will be used throughout Action for engine operations.
			mProfile->AddEngineAsync(engineSettings, enginePromise);
			mEngine = engineFuture.get();
		}

		// Creates a mip::FileHandler and returns to the caller. 
		// FileHandlers obtain a handle to a specific file, then perform any File API operations on the file.
		std::shared_ptr<mip::FileHandler> Action::CreateFileHandler(const std::string& filepath)
		{
			// Create promise/future for mip::FileHandler
			std::shared_ptr<std::promise<std::shared_ptr<mip::FileHandler>>> handlerPromise = std::make_shared<std::promise<std::shared_ptr<FileHandler>>>();
			std::future<std::shared_ptr<mip::FileHandler>> handlerFuture = handlerPromise->get_future();

			// Use mEngine::CreateFileHandlerAsync to create the handler
			// Filepath, the mip::FileHandler::Observer implementation, and the promise are required. 
			// Event notification will be provided to the appropriate function in the observer.
			// isAuditDiscoveryEnabled is set to true. This will generate discovery audits in AIP Analytics
			mEngine->CreateFileHandlerAsync(filepath, filepath, mGenerateAuditEvents, std::static_pointer_cast<FileHandler::Observer>(std::make_shared<FileHandlerObserver>()), handlerPromise);

			// Get the value and store in a mip::FileHandler object.
			// auto resolves to std::shared_ptr<mip::FileHandler>
			auto handler = handlerFuture.get();

			// return the pointer to mip::FileHandler to the caller
			return handler;
		}


		// Function recursively lists all labels available for a user to	std::cout.
		void Action::ListLabels() {

			// If mEngine hasn't been set, call AddNewFileEngine() to load the engine.
			if (!mEngine) {
				AddNewFileEngine();
			}

			// Use mip::FileEngine to list all labels
			auto labels = mEngine->ListSensitivityLabels();

			// Iterate through each label, first listing details
			for (const auto& label : labels) {
				cout << label->GetName() << " : " << label->GetId() << endl;

				// get all children for mip::Label and list details
				for (const auto& child : label->GetChildren()) {
					cout << "->  " << child->GetName() << " : " << child->GetId() << endl;
				}
			}
		}

		// Reads a label from the file at filepath, the displays.
		// Reading a label from a protected file will trigger consent flow, as implemented in mip::ConsentDelegate or derived classes.
		// In this sample, simple consent flow is implemented in consent_delegate_impl.h/cpp.
		void Action::ReadLabel(const std::string & filepath)		
		{
			cout << "Attempting to read label from output file." << endl;

			// Call private CreateFileHandler function, passing in file path. 
			// Returns a std::shared_ptr<mip::FileHandler> that will be used to read the label.
			auto handler = CreateFileHandler(filepath);

			// call mip::FileHandler::GetLabelAsync, passing in the promise.
			// The handler has the rest of the details it needs (file path and policy data via FileEngine) to display result.
			
			auto label = handler->GetLabel();						

			// Output results
			if (nullptr != label)
			{
				// Attempt to fetch parent label.
				auto parentLabel = std::shared_ptr<mip::Label>(label->GetLabel()->GetParent());
				
				// If parent exists, output parent \ child.
				if (nullptr != parentLabel)
				{										
					cout << "Name: " + parentLabel->GetName() + "\\" + label->GetLabel()->GetName() << endl;
					cout << "Id: " + label->GetLabel()->GetId() << endl;
				}
				// Else, output label info
				else
				{
					cout << "Name: " + label->GetLabel()->GetName() << endl;
					cout << "Id: " + label->GetLabel()->GetId() << endl;
				}
			}
			else
			{
				cout << "No label found." << endl;
			}
		}


		// Implements the code to assign a label to a file
		// Creates a file handler for filepath, sets the label with labelId, and writes the result to outputfile
		void Action::SetLabel(const std::string & filepath, const std::string& outputfile, const std::string & labelId)
		{
			// Call private CreateFileHandler function, passing in file path. 
			// Returns a std::shared_ptr<mip::FileHandler> that will be used to read the label.
			auto handler = CreateFileHandler(filepath);

			// Labeling requires a mip::LabelingOptions object. 
			// Review API ref for more details. The sample implies that the file was labeled manually by a user.
			mip::LabelingOptions labelingOptions(mip::AssignmentMethod::PRIVILEGED);

			// use the mip::FileHandler to set label with labelId and labelOptions created above
			handler->SetLabel(mEngine->GetLabelById(labelId), labelingOptions, mip::ProtectionSettings());
		
			// Changes to the file held by mip::FileHandler aren't committed until CommitAsync is called.						
			// Call Action::CommitChanges to write changes. Commit logic is implemented there.
			bool result = CommitChanges(handler, outputfile);

			// Write result to console.
			if (result) {
				cout << "Labeled: " + outputfile << endl;
			}
			else {
				cout << "Failed to label: " + outputfile << endl;
			}
		}

		// Implements code to commit changes made via mip::FileHandler
		// Accepts pointer to the mip::FileHandler and output file path
		bool Action::CommitChanges(const std::shared_ptr<mip::FileHandler>& fileHandler, const std::string& outputFile)
		{
			bool result = false;

			// Commit only if handler has been modified. Otherwise, return false.
			if (fileHandler->IsModified())
			{
				// CommitAsync is implemented similar to other async patterns via promise/future
				// In this instance, rather than a mip related object, we create the promise for a bool
				// The result provided will be true if the file was written, false if it failed.
				auto commitPromise = std::make_shared<std::promise<bool>>();
				auto commitFuture = commitPromise->get_future();

				// Commit changes to file referenced by fileHandler, writing to output file.
				fileHandler->CommitAsync(outputFile, commitPromise);
				result = commitFuture.get();

				// If flag is set to generate audit events, call mip::FileHandler::NotifyCommitSuccessful() to generate audit entry.
				if (mGenerateAuditEvents && result)
				{
					fileHandler->NotifyCommitSuccessful(outputFile);
				}
			}
			
			// Get value from future and return to caller. Will be true if operation succeeded, false otherwise.
			return result;
		}
	}
}
