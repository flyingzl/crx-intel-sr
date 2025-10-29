// Content script to inject the native API into web pages
(function() {
  'use strict';
  
  // Inject the API script into the page context
  const script = document.createElement('script');
  script.src = chrome.runtime.getURL('injected.js');
  script.onload = function() {
    this.remove();
  };
  
  // Inject as early as possible
  (document.head || document.documentElement).appendChild(script);
  
  // Listen for messages from the injected script
  window.addEventListener('message', function(event) {
    // Only accept messages from the same origin
    if (event.source !== window) return;
    
    if (event.data.type && event.data.type === 'FROM_PAGE_SCRIPT') {
      // Forward the message to background script
      chrome.runtime.sendMessage(
        {
          action: event.data.action,
          data: event.data.data
        },
        function(response) {
          // Send response back to the page
          window.postMessage({
            type: 'FROM_CONTENT_SCRIPT',
            requestId: event.data.requestId,
            response: response
          }, '*');
        }
      );
    }
  });
})();