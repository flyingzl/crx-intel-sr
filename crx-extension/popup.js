document.addEventListener('DOMContentLoaded', function() {
  const pingBtn = document.getElementById('pingBtn');
  const turnOnSRBtn = document.getElementById('turnOnSRBtn');
  const turnOffSRBtn = document.getElementById('turnOffSRBtn');
  const getSRStatusBtn = document.getElementById('getSRStatusBtn');
  const resultDiv = document.getElementById('result');

  // Set initial button states
  setButtonsEnabled(true);

  // Helper function to enable/disable buttons
  function setButtonsEnabled(enabled) {
    pingBtn.disabled = !enabled;
    turnOnSRBtn.disabled = !enabled;
    turnOffSRBtn.disabled = !enabled;
    getSRStatusBtn.disabled = !enabled;
  }

  // Helper function to display results
  function displayResult(data, isError = false) {
    resultDiv.style.display = 'block';
    resultDiv.className = isError ? 'error' : 'success';
    resultDiv.textContent = JSON.stringify(data, null, 2);
  }

  // Helper function to send messages to background script
  function sendMessage(action, data = {}) {
    setButtonsEnabled(false);
    
    chrome.runtime.sendMessage({
      action: action,
      data: data
    }, (response) => {
      setButtonsEnabled(true);
      
      if (chrome.runtime.lastError) {
        displayResult({
          error: chrome.runtime.lastError.message
        }, true);
      } else if (response && response.success) {
        displayResult(response.data);
      } else {
        displayResult({
          error: response ? response.error : 'Unknown error'
        }, true);
      }
    });
  }

  // Event listeners
  pingBtn.addEventListener('click', function() {
    sendMessage('ping');
  });

  turnOnSRBtn.addEventListener('click', function() {
    sendMessage('turn_on_sr', { mode: 0 });
  });

  turnOffSRBtn.addEventListener('click', function() {
    sendMessage('turn_off_sr');
  });

  getSRStatusBtn.addEventListener('click', function() {
    sendMessage('get_sr_status');
  });
});