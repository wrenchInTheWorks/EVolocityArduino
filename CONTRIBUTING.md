# Contributing to EVolocityLib

## Branching

Never commit directly to `master` — it is branch-protected and will reject direct pushes.

Create a branch with a descriptive prefix:

| Prefix | Use for |
|--------|---------|
| `feature/` | New classes or functionality |
| `fix/` | Bug fixes |
| `docs/` | README, wiki, or comment updates |
| `chore/` | CI, configuration, housekeeping |

## Pull Requests

Open a PR against `master`. All of the following checks must pass before merging:

- **Check Version Bump** — `library.properties` version must be incremented
- **Arduino Lint** — library structure must pass `arduino-lint` in `submit/strict` mode
- **Every class has an example** — each class defined in `src/` must appear in at least one example sketch
- **Compile all examples** — all sketches under `examples/` must compile cleanly against Arduino Nano (ATmega328P)

## Versioning

This library follows [Semantic Versioning](https://semver.org):

- `PATCH` (1.0.**x**) — bug fixes, no API changes
- `MINOR` (1.**x**.0) — new functionality, backwards compatible
- `MAJOR` (**x**.0.0) — breaking API changes

Bump the `version` field in `library.properties` as part of your PR. The release tag and GitHub Release are created automatically on merge.

## Adding a New Class

1. Create `src/ClassName.h` and `src/ClassName.cpp`
2. Add `#include "ClassName.h"` to `src/EVolocityLib.h`
3. Add at least one example under `examples/ClassNameBasicUsage/ClassNameBasicUsage.ino`
4. Add the class and its public methods to `keywords.txt`
