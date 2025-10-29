// Background script for handling native messaging
const NATIVE_HOST_NAME = 'com.miguvideo.sr';

// Handle messages from content scripts
chrome.runtime.onMessage.addListener((request, sender, sendResponse) => {
  if (request.action === 'ping') {
    chrome.runtime.sendNativeMessage(
      NATIVE_HOST_NAME,
      { action: 'ping' },
      (response) => {
        if (chrome.runtime.lastError) {
          console.error('Native messaging error:', chrome.runtime.lastError);
          sendResponse({
            success: false,
            error: chrome.runtime.lastError.message
          });
        } else {
          sendResponse({
            success: true,
            data: response.data
          });
        }
      }
    );
    return true;
  }
  
  if (request.action === 'turn_on_sr') {
    chrome.runtime.sendNativeMessage(
      NATIVE_HOST_NAME,
      { action: 'turn_on_sr', data: request.data },
      (response) => {
        if (chrome.runtime.lastError) {
          console.error('Native messaging error:', chrome.runtime.lastError);
          sendResponse({
            success: false,
            error: chrome.runtime.lastError.message
          });
        } else {
          sendResponse({
            success: true,
            data: response.data
          });
        }
      }
    );
    return true;
  }
  
  if (request.action === 'turn_off_sr') {
    chrome.runtime.sendNativeMessage(
      NATIVE_HOST_NAME,
      { action: 'turn_off_sr' },
      (response) => {
        if (chrome.runtime.lastError) {
          console.error('Native messaging error:', chrome.runtime.lastError);
          sendResponse({
            success: false,
            error: chrome.runtime.lastError.message
          });
        } else {
          sendResponse({
            success: true,
            data: response.data
          });
        }
      }
    );
    return true;
  }
  
  if (request.action === 'get_sr_status') {
    chrome.runtime.sendNativeMessage(
      NATIVE_HOST_NAME,
      { action: 'get_sr_status' },
      (response) => {
        if (chrome.runtime.lastError) {
          console.error('Native messaging error:', chrome.runtime.lastError);
          sendResponse({
            success: false,
            error: chrome.runtime.lastError.message
          });
        } else {
          sendResponse({
            success: true,
            data: response.data
          });
        }
      }
    );
    return true;
  }
});

// Extension installation handler
chrome.runtime.onInstalled.addListener((details) => {
  console.log('MiguVideo Super Resolution extension installed:', details.reason);
  testNativeConnection();
});

// Extension startup handler
chrome.runtime.onStartup.addListener(() => {
  console.log('MiguVideo Super Resolution extension started');
});

function testNativeConnection() {
  chrome.runtime.sendNativeMessage(
    NATIVE_HOST_NAME,
    { action: 'ping' },
    (response) => {
      if (chrome.runtime.lastError) {
        console.error('Failed to connect to native application:', chrome.runtime.lastError);
      } else {
        console.log('Successfully connected to native application:', response);
      }
    }
  );
}