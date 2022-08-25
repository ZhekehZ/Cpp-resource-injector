from conans import ConanFile, CMake
from os.path import join, exists
from os import environ


class ResourceInjectorConan(ConanFile):
    name = "resource-injector"
    version = "0.1"
    description = "A header only c++ library for resource injection"
    url = "https://github.com/ZhekehZ/Resource-Injector.git"
    license = "MIT"
    author = "Eugene Kravchenko"

    generators = "cmake"
    scm = dict(type="git", url=url,
               revision=environ.get('RESOURCE_INJECTOR_REVISION', 'master'))

    default_user = "zhekehz"
    default_channel = "stable"

    no_copy_source = True

    def _cmake(self):
        cmake = CMake(self)
        cmake.definitions["DO_NOT_TEST"] = "ON"
        cmake.configure()
        return cmake

    def package(self):
        cmake = self._cmake()
        cmake.install()

    def package_info(self):
        cmake_scripts = join(self.package_folder, "lib", "injector", "cmake", "injector")
        if exists(cmake_scripts):
            self.cpp_info.build_modules = [ join(cmake_scripts, "Injector.cmake") ]
