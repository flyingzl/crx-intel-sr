#!/usr/bin/env python3
import json
import struct
import subprocess
import sys

def send_message(proc, message):
    """发送消息到native应用"""
    message_json = json.dumps(message)
    message_bytes = message_json.encode('utf-8')
    length = len(message_bytes)
    
    # 发送长度（4字节，小端序）
    proc.stdin.write(struct.pack('<I', length))
    # 发送消息内容
    proc.stdin.write(message_bytes)
    proc.stdin.flush()

def read_message(proc):
    """从native应用读取消息"""
    # 读取长度（4字节）
    length_bytes = proc.stdout.read(4)
    if len(length_bytes) != 4:
        return None
    
    length = struct.unpack('<I', length_bytes)[0]
    
    # 读取消息内容
    message_bytes = proc.stdout.read(length)
    if len(message_bytes) != length:
        return None
    
    return json.loads(message_bytes.decode('utf-8'))

def test_native_app():
    """测试native应用"""
    exe_path = r"sr-native\target\release\sr-native.exe"
    
    try:
        # 启动native应用
        proc = subprocess.Popen(
            exe_path,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        
        print("测试 ping...")
        send_message(proc, {"action": "ping"})
        response = read_message(proc)
        print(f"ping 响应: {response}")
        
        print("\n测试 turn_on_sr...")
        send_message(proc, {"action": "turn_on_sr", "data": {"mode": 0}})
        response = read_message(proc)
        print(f"turn_on_sr 响应: {response}")
        
        print("\n测试 turn_off_sr...")
        send_message(proc, {"action": "turn_off_sr"})
        response = read_message(proc)
        print(f"turn_off_sr 响应: {response}")
        
        # 关闭进程
        proc.stdin.close()
        proc.wait()
        
        # 检查stderr输出
        stderr_output = proc.stderr.read().decode('utf-8')
        if stderr_output:
            print(f"\n调试输出:\n{stderr_output}")
            
    except Exception as e:
        print(f"测试失败: {e}")

if __name__ == "__main__":
    test_native_app()