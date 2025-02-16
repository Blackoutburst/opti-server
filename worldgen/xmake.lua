add_rules("mode.debug", "mode.release")

set_languages("c++20")

package("fastnoise2")
    -- set_kind("static")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "lib/fastnoise2"))
    add_includedirs("include")

    -- add_defines("FASTNOISE_STATIC_LIB")

    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=Release")
        table.insert(configs, "-DBUILD_SHARED_LIBS=OFF")
        -- table.insert(configs, "-DBUILD_STATIC_LIBS=ON")
        table.insert(configs, "-DFASTNOISE2_NOISETOOL=OFF")
        table.insert(configs, "-DFASTNOISE2_TESTS=OFF")
        -- table.insert(configs, "-DFASTNOISE_STATIC_LIB=1")

        import("package.tools.cmake").install(package, configs)
    end)
package_end()

add_requires("fastnoise2")

target("worldgen")
    set_prefixname("") -- To remove the prefix lib of shared library
    set_kind("shared")
    -- set_kind("binary")

    -- add_defines("FASTNOISE_STATIC_LIB")

    -- add_cxflags("-W -Wall -Wextra -Wpedantic -Wno-unused-parameter")
    set_optimize("fastest")
    add_files("src/*.cpp")
    add_packages("fastnoise2")


    after_build(function (target)
        print("Built target: %s %s", target:name(), target:targetfile())
        -- print("Target: %s %s", target:name(), target:targetfile())
        -- print(target:targetfile())
        -- print(target:targetDirectory())
        os.mv(target:targetfile(), "../")
    end)
