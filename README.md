# build2 Package for GSL

This project is a [build2](https://build2.org) package repository that provides access to the [Guidelines Support Library (GSL)](https://github.com/microsoft/GSL), a C++ header-only library that contains functions and types that are suggested for use by the [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines) maintained by the Standard C++ Foundation.

[![Official](https://img.shields.io/website/https/github.com/microsoft/gsl.svg?down_message=offline&label=Official&style=for-the-badge&up_color=blue&up_message=online)](https://github.com/microsoft/gsl)
[![build2](https://img.shields.io/website/https/github.com/build2-packaging/gsl.svg?down_message=offline&label=build2&style=for-the-badge&up_color=blue&up_message=online)](https://github.com/build2-packaging/gsl)
[![cppget.org](https://img.shields.io/website/https/cppget.org/gsl.svg?down_message=offline&label=cppget.org&style=for-the-badge&up_color=blue&up_message=online)](https://cppget.org/gsl)
[![queue.cppget.org](https://img.shields.io/website/https/queue.cppget.org/gsl.svg?down_message=empty&down_color=blue&label=queue.cppget.org&style=for-the-badge&up_color=orange&up_message=running)](https://queue.cppget.org/gsl)

## Usage
Make sure to add the stable section of the `cppget.org` repository to your project's `repositories.manifest` to be able to fetch this package.

    :
    role: prerequisite
    location: https://pkg.cppget.org/1/stable
    # trust: ...

If the stable section of `cppget.org` is not an option then add this Git repository itself instead as a prerequisite.

    :
    role: prerequisite
    location: https://github.com/build2-packaging/gsl.git

Add the respective dependency in your project's `manifest` file to make the package available for import.

    depends: gsl ^4.0.0

The library can be imported by the following declaration in a `buildfile`.

    import gsl = gsl%lib{gsl}

## Configuration
There are no configuration options available.

## Issues and Notes
- All test source files are not provided as symbolic links but have been deeply copied and altered to be executable with `doctest` and not `gtest`. At the time of creation, `gtest` was not available. For the next package version, this will most likely be changed. The tests will be provided as external tests packages depending on `gtest`.

## Contributing
Thank you in advance for your help and contribution to keep this package up-to-date.
Please, file an issue on [GitHub](https://github.com/build2-packaging/gsl/issues) for questions, bug reports, or to recommend updating the package version.
If you're making a pull request to fix bugs or update the package version yourself, refer to the [`build2` Packaging Guidelines](https://build2.org/build2-toolchain/doc/build2-toolchain-packaging.xhtml#core-version-management).
