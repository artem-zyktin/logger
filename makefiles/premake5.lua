rootdir  = '../'
builddir = rootdir .. 'build/'
bindir   = rootdir .. 'bin/'
srcdir   = rootdir .. 'src/'

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

project 'logger_test'
	kind 'ConsoleApp'
	language 'C++'
	cppdialect 'C++20'
	targetdir (outputdir)
	objdir (intermadiatedir)

	test_srcdir = srcdir .. 'test/'

	files {
		test_srcdir .. '**.h',
		test_srcdir .. '**.hpp',
		test_srcdir .. '**.cpp'
	}

	includedirs {
		srcdir
	}

    links { 'logger' }

    libdirs { libdir }

	filter 'configurations:Debug'
		defines { '_DEBUG' }
		symbols 'On'

	filter 'configurations:Release'
		defines { 'NDEBUG' }
		optimize 'On'
