fn main() {
    // 只有在 Windows 平台才尝试链接 SR 库
    #[cfg(target_os = "windows")]
    {
        use std::env;
        use std::path::PathBuf;
        use std::fs;
        
        // 获取当前目录
        let manifest_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
        let sr_lib_dir = PathBuf::from(&manifest_dir).join("sr_lib");
        
        // 设置库搜索路径
        let lib_dir = sr_lib_dir.join("bin");
        println!("cargo:rustc-link-search=native={}", lib_dir.display());
        
        // 设置头文件搜索路径
        let include_dir = sr_lib_dir.join("include");
        println!("cargo:include={}", include_dir.display());
        
        // 链接 sr_lib 动态库 (DLL)
        println!("cargo:rustc-link-lib=dylib=sr_lib");
        
        // 获取输出目录
        let out_dir = env::var("OUT_DIR").unwrap();
        let out_path = PathBuf::from(&out_dir);
        let target_dir = out_path
            .parent().unwrap()
            .parent().unwrap()
            .parent().unwrap();
        
        // 复制 DLL 到输出目录
        let dll_src = lib_dir.join("sr_lib.dll");
        let dll_dst = target_dir.join("sr_lib.dll");
        
        if dll_src.exists() {
            if let Err(e) = fs::copy(&dll_src, &dll_dst) {
                println!("cargo:warning=Failed to copy sr_lib.dll: {}", e);
            } else {
                println!("cargo:warning=Successfully copied sr_lib.dll to {}", dll_dst.display());
            }
        } else {
            println!("cargo:warning=sr_lib.dll not found at {}", dll_src.display());
        }
    }
    
    // 告诉 Cargo 当这些文件改变时重新构建
    println!("cargo:rerun-if-changed=sr_lib/");
    println!("cargo:rerun-if-changed=build.rs");
}