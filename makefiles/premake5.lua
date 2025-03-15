rootdir       = '../'
builddir      = rootdir .. 'build/'
bindir        = rootdir .. 'bin/'
srcdir        = rootdir .. 'src/'
thirdpartydir = srcdir .. 'thirdparty/'

configdir       = bindir .. '%{cfg.architecture}/%{cfg.buildcfg}/'
intermadiatedir = configdir .. 'intermediate/'
libdir          = configdir .. 'lib/'
outputdir       = configdir .. 'output/'

workspace 'Logger'
	configurations { 'Debug', 'Release' }
	architecture 'x64'
	location '../build/%{_ACTION}'

project 'logger'
	kind 'StaticLib'
	language 'C++'
	cppdialect 'C++20'
	targetdir (libdir)
	objdir (intermadiatedir)

	logger_srcdir = srcdir .. 'logger/'

	includedirs {
		thirdpartydir
	}

	files {
		logger_srcdir .. '**.h',
		logger_srcdir .. '**.hpp',
		logger_srcdir .. '**.cpp',
	}

	filter 'configurations:Debug'
		defines { '_DEBUG' }
		symbols 'On'

	filter 'configurations:Release'
		defines { 'NDEBUG' }
		optimize 'On'

project 'gtest'
	kind 'StaticLib'
	language 'C++'
	cppdialect 'C++20'
	targetdir (libdir)
	objdir (intermadiatedir)

	includedirs {
		thirdpartydir .. 'googletest/',
		thirdpartydir .. 'googletest/include/'
	}

	files {
		thirdpartydir .. 'googletest/src/gtest-all.cc'
	}

	defines { 'GTEST_HAS_PTHREAD=0' }

	filter 'configurations:Debug'
		defines { '_DEBUG' }
		symbols 'On'

	filter 'configurations:Release'
		defines { 'NDEBUG' }
		optimize 'On'

project 'logger_test'
	kind 'ConsoleApp'
	language 'C++'
	cppdialect 'C++20'
	targetdir (outputdir)
	objdir (intermadiatedir)

	includedirs {
		srcdir,
		thirdpartydir .. 'googletest/include/'
	}

	logger_test_srcdir = srcdir .. 'test/'
	files {
		logger_test_srcdir .. '**.cpp'
	}

	links { 'logger', 'gtest' }
	libdirs { libdir }

	filter 'configurations:Debug'
		defines { '_DEBUG' }
		symbols 'On'

	filter 'configurations:Release'
		defines { 'NDEBUG' }
		optimize 'On'
