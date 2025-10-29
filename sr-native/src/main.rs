use anyhow::{Context, Result};
use serde::{Deserialize, Serialize};
use std::io::{self, Read, Write};
use gag::Gag;

// Windows 平台的 SR 库外部函数声明
#[cfg(target_os = "windows")]
unsafe extern "C" {
    fn SuperResolutionInitialize() -> bool;
    fn SuperResolutionRelease(); // 注意：返回void，不是bool
    fn SuperResolutionSet(enable: bool, mode: i32) -> bool;
    fn SuperResolutionGetStatus() -> i32;
}

// Windows 平台的 I/O 模式设置
#[cfg(target_os = "windows")]
unsafe extern "C" {
    fn _setmode(fd: i32, mode: i32) -> i32;
}

#[cfg(target_os = "windows")]
const _O_BINARY: i32 = 0x8000;

#[derive(Deserialize, Debug)]
struct Request {
    action: String,
    #[serde(default)]
    data: serde_json::Value,
}

#[derive(Serialize, Debug)]
struct Response {
    success: bool,
    #[serde(skip_serializing_if = "Option::is_none")]
    data: Option<serde_json::Value>,
    #[serde(skip_serializing_if = "Option::is_none")]
    error: Option<String>,
}

#[derive(Serialize, Debug)]
struct SrStatus {
    status: i32,
    enabled: bool,
    message: String,
}

fn main() -> Result<()> {
    Gag::stderr().context("Failed to redirect stderr")?;
    Gag::stdout().context("Failed to redirect stderr")?;
    // Windows 平台设置二进制 I/O 模式
    #[cfg(target_os = "windows")]
    unsafe {
        // 设置 stdin (0) 和 stdout (1) 为二进制模式
        _setmode(0, _O_BINARY); // stdin
        _setmode(1, _O_BINARY); // stdout
    }
    
    loop {
        match read_message() {
            Ok(request) => {
                let response = handle_request(request);
                if let Err(_e) = write_message(&response) {
                    break;
                }
            }
            Err(_e) => {
                break;
            }
        }
    }
    Ok(())
}

fn read_message() -> Result<Request> {
    let mut length_bytes = [0u8; 4];
    io::stdin()
        .read_exact(&mut length_bytes)
        .context("Failed to read message length")?;

    let length = u32::from_le_bytes(length_bytes) as usize; // 明确使用小端序
    
    // 检查消息长度限制
    if length > 1024 * 1024 {
        return Err(anyhow::anyhow!("Message length {} exceeds maximum allowed size", length));
    }
    
    let mut buffer = vec![0u8; length];
    io::stdin()
        .read_exact(&mut buffer)
        .context("Failed to read message content")?;

    let request: Request = serde_json::from_slice(&buffer)
        .context("Failed to parse JSON message")?;

    Ok(request)
}

fn write_message(response: &Response) -> Result<()> {
    let json = serde_json::to_string(response)
        .context("Failed to serialize response")?;
    
    let message_bytes = json.as_bytes();
    let length = message_bytes.len() as u32;
    
    // 检查消息长度限制
    if length > 1024 * 1024 {
        return Err(anyhow::anyhow!("Response length {} exceeds maximum allowed size", length));
    }
    
    let length_bytes = length.to_le_bytes(); // 明确使用小端序
    
    io::stdout().write_all(&length_bytes)
        .context("Failed to write message length")?;
    io::stdout().write_all(message_bytes)
        .context("Failed to write message content")?;
    io::stdout().flush()
        .context("Failed to flush stdout")?;
    
    Ok(())
}

fn handle_request(request: Request) -> Response {
    match request.action.as_str() {
        "ping" => {
            let sr_support = cfg!(target_os = "windows");
            Response {
                success: true,
                data: Some(serde_json::json!({
                    "message": "pong",
                    "sr_support": sr_support
                })),
                error: None,
            }
        }
        "turn_on_sr" => {
            let status = turn_on_sr(&request.data);
            Response {
                success: true,
                data: Some(serde_json::to_value(status).unwrap()),
                error: None,
            }
        }
        "turn_off_sr" => {
            let status = turn_off_sr();
            Response {
                success: true,
                data: Some(serde_json::to_value(status).unwrap()),
                error: None,
            }
        }
        "get_sr_status" => {
            let status = get_sr_status();
            Response {
                success: true,
                data: Some(serde_json::to_value(status).unwrap()),
                error: None,
            }
        }
        _ => Response {
            success: false,
            data: None,
            error: Some(format!("Unknown action: {}", request.action)),
        },
    }
}

// Windows 平台的 SR 功能实现 - 完全按照demo_node_npi.cpp的逻辑
#[cfg(target_os = "windows")]
fn turn_on_sr(data: &serde_json::Value) -> SrStatus {
    let mode = data.get("mode")
        .and_then(|v| v.as_i64())
        .unwrap_or(0) as i32;

    unsafe {
        // 按照demo_node_npi.cpp: 直接调用Initialize，不判断结果
        SuperResolutionInitialize();
        
        // 主要判断SuperResolutionSet的结果
        let success = SuperResolutionSet(true, mode);
        
        // 调用Release
        SuperResolutionRelease();
        
        // 根据SuperResolutionSet的结果返回状态
        if success {
            SrStatus {
                status: 1,
                enabled: true,
                message: format!("SuperResolution enabled with mode {}", mode),
            }
        } else {
            SrStatus {
                status: -1,
                enabled: false,
                message: "Failed to enable SuperResolution".to_string(),
            }
        }
    }
}

#[cfg(target_os = "windows")]
fn turn_off_sr() -> SrStatus {
    unsafe {
        // 按照demo_node_npi.cpp: 直接调用Initialize，不判断结果
        SuperResolutionInitialize();
        
        // 主要判断SuperResolutionSet的结果
        let success = SuperResolutionSet(false, 0);
        
        // 调用Release
        SuperResolutionRelease();
        
        // 根据SuperResolutionSet的结果返回状态
        if success {
            SrStatus {
                status: 0,
                enabled: false,
                message: "SuperResolution disabled".to_string(),
            }
        } else {
            SrStatus {
                status: -1,
                enabled: false,
                message: "Failed to disable SuperResolution".to_string(),
            }
        }
    }
}

#[cfg(target_os = "windows")]
fn get_sr_status() -> SrStatus {
    unsafe {
        // 按照demo_node_npi.cpp: 直接调用Initialize，不判断结果
        SuperResolutionInitialize();
        
        // 获取状态
        let status = SuperResolutionGetStatus();
        
        // 调用Release
        SuperResolutionRelease();
        
        let enabled = status > 0;
        SrStatus {
            status,
            enabled,
            message: if enabled {
                format!("SuperResolution is enabled with status {}", status)
            } else {
                "SuperResolution is disabled".to_string()
            },
        }
    }
}

// 非 Windows 平台的 SR 功能实现（直接返回 false）
#[cfg(not(target_os = "windows"))]
fn turn_on_sr(_data: &serde_json::Value) -> SrStatus {
    SrStatus {
        status: -1,
        enabled: false,
        message: "SuperResolution not supported on this platform".to_string(),
    }
}

#[cfg(not(target_os = "windows"))]
fn turn_off_sr() -> SrStatus {
    SrStatus {
        status: -1,
        enabled: false,
        message: "SuperResolution not supported on this platform".to_string(),
    }
}

#[cfg(not(target_os = "windows"))]
fn get_sr_status() -> SrStatus {
    SrStatus {
        status: -1,
        enabled: false,
        message: "SuperResolution not supported on this platform".to_string(),
    }
}