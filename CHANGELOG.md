# Changelog
All notable changes to PBuf will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Fixed
- Some refactorings

### Added
- n/a

### Changed
- n/a

## [0.2.0] - 06-03-2019

### Fixed
- n/a

### Added
- Headless Mode. Configurable by defining `EXTERNAL_DATA_BUFFER`. In this mode the user maintains an external payload buffer, and *PBuf* provides prioritised access via the API commands `PBUF_insertIndex()` and `PBUF_retrieveIndex()`. Added relevant documentation.

- Added this CHANGELOG

### Changed
- n/a

## [0.1.0] - 28-02-2019

### Fixed

- Mapping bugs

### Added

- Initial Buffer and Priority management code
