var func_status = 0; // 0: ready, 1: working, 2: finish
var input_buffer = ''; // async input buffer

document.addEventListener('DOMContentLoaded', () => {
  // Get DOM elements
  const textarea = document.querySelector('textarea.shell');
  const input = document.querySelector('#box_input');
  const enterBtn = document.querySelector('#btn_enter');

  // Make textarea readonly
  textarea.readOnly = true;

  // Bind functions to buttons
  document.querySelector('#btn_play').addEventListener('click', () => {
    if (func_status == 0 || func_status == 2) {
      input_buffer = '';
      set_status(1);
      exec_code();
    }
  });
  document.querySelector('#btn_stop').addEventListener('click', () => {
    if (func_status == 1) {
      set_status(0);
    }
  });
  enterBtn.addEventListener('click', () => {
    if (func_status == 1) {
      input_buffer = input.value + '\n';
      print_text(input_buffer);
      input.value = '';
    }
  });

  // Handle enter key in input
  input.addEventListener('keypress', (e) => {
    if (e.key === 'Enter') {
      e.preventDefault();
      enterBtn.click();
    }
  });
});

// set status sign
function set_status(s) {
  const status = document.querySelector('#status');
  func_status = s;
  if (s == 0) {
    status.innerHTML = 'ready';
  } else if (s == 1) {
    status.innerHTML = 'working';
  } else {
    status.innerHTML = 'finish';
  }
}

// system call : get text
async function get_text() {
  return new Promise(resolve => {
    const checkBuffer = () => {
      if (input_buffer == '') {
        setTimeout(checkBuffer, 100);
      } else {
        const text = input_buffer;
        input_buffer = '';
        resolve(text);
      }
    };
    checkBuffer();
  });
}

// system call : print text
function print_text(text) {
  const textarea = document.querySelector('textarea.shell');
  textarea.value += text;
}

// object code executer
async function exec_code() {
  await c_x2();
  await c_x2();
  set_status(2);
}

async function c_x2(t) {
  var t = await get_text();
  print_text(t);
}