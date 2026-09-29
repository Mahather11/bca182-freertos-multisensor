import os

Import("env")

toolchain = env.PioPlatform().get_package_dir("toolchain-gccmingw32")
if toolchain and os.path.isdir(os.path.join(toolchain, "bin")):
    env.PrependENVPath("PATH", os.path.join(toolchain, "bin"))

env.Append(CXXFLAGS=["-std=gnu++14"])
env.Append(LINKFLAGS=["-static", "-static-libgcc", "-static-libstdc++"])