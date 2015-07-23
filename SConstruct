import os
import sys
import glob
import shutil
import excons
from excons.tools import unity
from excons.tools import tbb
from excons.tools import dl

use_externals = (sys.platform == "win32" and excons.Build64() and excons.GetArgument("use-externals", 1, int) != 0)
if use_externals:
  # Provided externals are built using runtime version 12.0
  # 'mscver' has to be set before excons.MakeBaseEnv is called for the toolchain to be properly setup by SCons
  excons.SetArgument("mscver", "12.0")

env = excons.MakeBaseEnv()

# I don't know whst this whole PatchLibrary is. Looks like a hack that we don't
# really need. Let's disable it for now by defining aiMaster
defines = ["aiMaster"]
inc_dirs = ["AlembicImporterPlugin"]
lib_dirs = []
libs = []
embed_libs = []
customs = []
install_files = {"unity/AlembicImporter/Scripts": glob.glob("AlembicImporter/Assets/AlembicImporter/Scripts/*.cs"),
                 "unity/AlembicImporter/Editor": glob.glob("AlembicImporter/Assets/AlembicImporter/Editor/*.cs"),
                 "unity/AlembicImporter/Shaders": glob.glob("AlembicImporter/Assets/AlembicImporter/Shaders/*.shader")}
sources = filter(lambda x: os.path.basename(x) not in ["pch.cpp"], glob.glob("AlembicImporterPlugin/*.cpp"))
sources.extend(glob.glob("AlembicImporterPlugin/Schema/*.cpp"))
sources.extend(["AlembicImporterPlugin/GraphicsDevice/aiGraphicsDevice.cpp"])

if excons.GetArgument("debug", 0, int) != 0:
  defines.append("aiDebug")

if excons.GetArgument("opengl", 0, int) != 0:
  defines.append("aiSupportOpenGL")
  # device source?

if use_externals:
  # we're in windows
  if excons.GetArgument("d3d9", 0, int) != 0:
    defines.append("aiSupportD3D9")
    # device source?

  if excons.GetArgument("d3d11", 1, int) != 0:
    defines.append("aiSupportD3D11")
    sources.append("AlembicImporterPlugin/GraphicsDevice/aiGraphicsDeviceD3D11.cpp")
  
  inc_dirs.extend(["AlembicImporterPlugin/external/ilmbase-2.2.0/Half",
                   "AlembicImporterPlugin/external/ilmbase-2.2.0/Iex",
                   "AlembicImporterPlugin/external/ilmbase-2.2.0/IexMath",
                   "AlembicImporterPlugin/external/ilmbase-2.2.0/Imath",
                   "AlembicImporterPlugin/external/ilmbase-2.2.0/IlmThread",
                   "AlembicImporterPlugin/external/ilmbase-2.2.0/config",
                   "AlembicImporterPlugin/external/hdf5-1.8.14/hl/src",
                   "AlembicImporterPlugin/external/hdf5-1.8.14/src",
                   "AlembicImporterPlugin/external/hdf5-1.8.14",
                   "AlembicImporterPlugin/external/alembic-1_05_08/lib"])
  
  lib_dirs.append("AlembicImporterPlugin/external/libs/x86_64")
  
  # Cleanup build using custom alembic
  inc_dir = os.path.join(excons.OutputBaseDirectory(), "include")
  if os.path.isdir(inc_dir):
    shutil.rmtree(inc_dir)

  lib_dir = os.path.join(excons.OutputBaseDirectory(), "lib")
  if os.path.isdir(lib_dir):
    shutil.rmtree(lib_dir)

  dll_pattern = os.path.join(excons.OutputBaseDirectory(), "unity", "AlembicImporter", "Plugins", "x86_64", "*.dll")
  keep_names = ["AlembicImporter"]
  dlls_to_remove = filter(lambda x: os.path.splitext(os.path.basename(x))[0] not in keep_names, glob.glob(dll_pattern))
  for dll in dlls_to_remove:
    os.remove(dll)

else:
  SConscript("alembic/SConstruct")
  
  Import("RequireAlembic")
  
  customs.append(RequireAlembic())
  
  defines.append("aiNoAutoLink")

  if excons.GetArgument("tbb", 0, int) != 0:
    defines.append("aiWithTBB")
    customs.append(tbb.Require)
  
  if sys.platform == "win32":
    if excons.GetArgument("d3d9", 0, int) != 0:
      defines.append("aiSupportD3D9")
      # device source?

    if excons.GetArgument("d3d11", 1, int) != 0:
      defines.append("aiSupportD3D11")
      sources.append("AlembicImporterPlugin/GraphicsDevice/aiGraphicsDeviceD3D11.cpp")
  
  embed_libs = excons.GetArgument("embed-libs", [])
  if embed_libs:
    if os.path.isdir(embed_libs):
      pat = ("/*.dylib" if sys.platform == "darwin" else ("/*.dll" if sys.platform == "win32" else "/*.so"))
      embed_libs = glob.glob(embed_libs + pat)
    else:
      embed_libs = [embed_libs]

importer = {"name": "AlembicImporter",
            "type": "dynamicmodule",
            "defs": defines,
            "incdirs": inc_dirs,
            "libdirs": lib_dirs,
            "libs": libs,
            "custom": customs,
            "srcs": sources,
            "install": install_files}

unity.Plugin(importer, libs=embed_libs)

tester = {"name": "tester",
          "type": "program",
          "custom": [dl.Require],
          "srcs": ["TestData/tester.cpp"]}

targets = [importer, tester]

excons.DeclareTargets(env, targets)

Default(["AlembicImporter"])
