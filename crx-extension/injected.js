// Injected script that provides the native API to web pages
(function() {
  'use strict';
  
  let requestId = 0;
  const pendingRequests = new Map();
  
  // Listen for responses from content script
  window.addEventListener('message', function(event) {
    if (event.source !== window) return;
    
    if (event.data.type && event.data.type === 'FROM_CONTENT_SCRIPT') {
      const { requestId: id, response } = event.data;
      const { resolve, reject } = pendingRequests.get(id) || {};
      
      if (resolve && reject) {
        pendingRequests.delete(id);
        
        if (response.success) {
          resolve(response.data);
        } else {
          reject(new Error(response.error || 'Unknown error'));
        }
      }
    }
  });
  
  // Helper function to send messages to content script
  function sendMessage(action, data = {}) {
    return new Promise((resolve, reject) => {
      const currentRequestId = ++requestId;
      
      pendingRequests.set(currentRequestId, { resolve, reject });
      
      // Set timeout for requests
      setTimeout(() => {
        if (pendingRequests.has(currentRequestId)) {
          pendingRequests.delete(currentRequestId);
          reject(new Error('Request timeout'));
        }
      }, 10000); // 10 second timeout
      
      window.postMessage({
        type: 'FROM_PAGE_SCRIPT',
        action: action,
        data: data,
        requestId: currentRequestId
      }, '*');
    });
  }
  
  // Create the native API object
  const nativeApi = {
    // Check if the API is available
    isAvailable: function() {
      return true;
    },
    
    // Get extension version
    getVersion: function() {
      return '1.0.0';
    },
    
    // Test connection to native application
    ping: function() {
      return sendMessage('ping');
    },
    
    // Turn on Super Resolution
    turnOnSR: function(mode = 0) {
      return sendMessage('turn_on_sr', { mode: mode });
    },
    
    // Turn off Super Resolution
    turnOffSR: function() {
      return sendMessage('turn_off_sr');
    },
    
    // Get Super Resolution status
    getSRStatus: function() {
      return sendMessage('get_sr_status');
    }
  };
  
  // Expose the API to the global window object
  window.nativeApi = nativeApi;
  
  // Dispatch a custom event to notify that the API is ready
  window.dispatchEvent(new CustomEvent('nativeApiReady', {
    detail: { api: nativeApi }
  }));
  
  console.log('MiguVideo Super Resolution API injected successfully');
})();