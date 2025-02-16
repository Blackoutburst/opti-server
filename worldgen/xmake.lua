add_rules("mode.debug", "mode.release")

set_languages("c23", "c++20")


package("fastnoise2")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "lib/fastnoise2"))
    print(path.join(os.scriptdir(), "lib/fastnoise2"))
-- add_includedirs("$(buildir)/submodules/fastnoise2/include")

    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
    -- on_test(function (package)
    --     assert(package:has_cfuncs("add", {includes = "foo.h"}))
    -- end)
package_end()

-- add_requires("fastnoise2")

-- target("worldgen")
--     set_kind("shared")
--     add_cflags("-std=c23")
--     -- add_cxflags("-W -Wall -Wextra -Wpedantic -Wno-unused-parameter")
--     set_optimize("fastest")
--     add_files("src/*.c")
--     add_packages("fastnoise2")

--     after_build(function (target)
--         print("hello: %s %s", target:name(), target:targetfile())
--         -- print("Target: %s %s", target:name(), target:targetfile())
--         os.mv(target:targetfile(), "../")
--     end)

-- https://xmake.io/mirror/plugin/builtin_plugins.html

-- xmake f -p mingw

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--
