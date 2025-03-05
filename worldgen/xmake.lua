add_rules("mode.debug", "mode.release")

set_languages("c++20")

package("fastnoise2")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "lib/fastnoise2"))
    add_includedirs("include")

    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=Release")
        table.insert(configs, "-DBUILD_SHARED_LIBS=OFF")
        table.insert(configs, "-DFASTNOISE2_NOISETOOL=OFF")
        table.insert(configs, "-DFASTNOISE2_TESTS=OFF")

        import("package.tools.cmake").install(package, configs)
    end)
package_end()

package("glm")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "lib/glm"))

    on_load(function (package)
        if package:config("modules") then
            package:config_set("header_only", false)
            package:config_set("cxx_standard", "20")
        elseif package:config("header_only") then
            package:set("kind", "library", {headeronly = true})
        else
            package:add("deps", "cmake")
        end
    end)

    on_install(function (package)
        if package:config("header_only") then
            os.cp("glm", package:installdir("include"))
        else
            io.replace("CMakeLists.txt", "NOT GLM_DISABLE_AUTO_DETECTION", "FALSE")
            local configs = {"-DGLM_BUILD_TESTS=OFF"}
            table.insert(configs, "-DBUILD_SHARED_LIBS=OFF")
            table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
            table.insert(configs, "-DCMAKE_CXX_STANDARD=20")
            import("package.tools.cmake").install(package, configs)
        end
    end)
package_end()

add_requires("fastnoise2")
add_requires("glm")

target("worldgen")
    set_prefixname("") -- To remove the prefix lib of shared library
    set_kind("shared")
    add_defines("MYLIB_EXPORTS")

    add_includedirs("include")

    add_defines("GLM_ENABLE_EXPERIMENTAL")

    -- add_cxflags("-W -Wall -Wextra -Wpedantic -Wno-unused-parameter")
    set_optimize("fastest")
    add_files("src/*.cpp", "src/**/*.cpp")

    add_packages("fastnoise2")
    add_packages("glm")

    after_build(function (target)
        print("Built target: %s %s", target:name(), target:targetfile())
        os.mv(target:targetfile(), "../")
    end)
