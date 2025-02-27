add_rules("mode.debug", "mode.release")

set_languages("clatest", "c++20")

if is_mode("release") then
    set_optimize("fastest")
end

target("server")

    set_kind("binary")
    -- add_cflags("-std=c23")
    add_cxflags("-W", "-Wall", "-Wextra", "-Wpedantic", "-Wno-unused-parameter")
    add_includedirs("includes")
    add_files("src/*.c", "src/**/*.c")

    if is_host("windows") then
        add_ldflags("-Llib -lws2_32")
    end

    if is_host("linux") then
        add_cxflags("-Wno-tautological-compare")
        add_ldflags("-pthread")
    end

    if is_mode("release") then
        add_cxflags("-O3")
    end

    if is_mode("debug") then
        set_policy("build.sanitizer.address", true)
        add_cxflags("-g3", "-fno-omit-frame-pointer")
        -- add_defines("DEBUG")
    end

    if is_mode("release") then
        set_policy("build.sanitizer.address", false)
    end

    after_build(function (target)
        -- os.mv(target:targetfile(), "../")
        os.cp(target:targetfile(), "../")
    end)

-- xmake f -p mingw
