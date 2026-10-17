.. _getting-started:

Getting Started
===============

Prerequisites
-------------

.. list-table::
   :header-rows: 1
   :widths: 25 75

   * - Requirement
     - Notes
   * - C++20 compiler
     - GCC 10+ or Clang 13+. The code uses ``<numbers>``, designated
       initializers, ``std::optional``, and ``[[nodiscard]]``. Apple Clang from
       the Xcode Command Line Tools works.
   * - GNU Make
     - 3.81 or newer. The version that ships with macOS is fine.
   * - Git
     - Needed to fetch the GoogleTest submodule for the test build.

Building the documentation additionally needs Python 3 and PlantUML; see
:ref:`building-docs`.

Getting the source
------------------

.. code-block:: sh

   git clone --recurse-submodules <repo-url>
   cd KSP_RelayOrbiter_calculator

``--recurse-submodules`` is optional: the test build fetches GoogleTest on
demand (see :ref:`googletest-submodule`).

Building
--------

The Makefile lives in ``build/``. Run it from the repository root with
``make -C build <target>``, or from inside ``build/`` with ``make <target>``.

.. code-block:: sh

   make -C build          # build the app and the tests, then run the tests
   make -C build app      # build only the application
   make -C build test     # build and run only the tests
   make -C build html     # build this documentation
   make -C build clean    # delete build/obj/

.. list-table:: Make targets
   :header-rows: 1
   :widths: 20 80

   * - Target
     - Description
   * - ``all`` *(default)*
     - Builds the application and the test binary, then runs the tests.
   * - ``app``
     - Builds ``build/obj/relay_calculator``. Does **not** need GoogleTest.
   * - ``test``
     - Builds ``build/obj/run_tests`` and runs it. Fetches the GoogleTest
       submodule first if it is missing.
   * - ``html``
     - Builds this documentation by delegating to ``docs/Makefile``; see
       :ref:`building-docs`.
   * - ``docs-clean``
     - Removes ``docs/_build``.
   * - ``clean``
     - Removes ``build/obj/`` — every object file and binary.

All outputs land in ``build/obj/``, which is git-ignored.

.. uml:: diagrams/build_flow.puml
   :caption: What each make target does

Compiler flags
~~~~~~~~~~~~~~

Everything is compiled with::

   -std=c++20 -Wall -Wextra -I hpp -I utest -I <googletest include dirs>

The build is warning-free on Apple Clang 21.

Choosing a compiler
~~~~~~~~~~~~~~~~~~~

The Makefile defaults to ``g++``. Override it on the command line:

.. code-block:: sh

   make -C build CXX=clang++
   make -C build CXX=g++-14

Run ``make -C build clean`` after switching compilers so stale objects are
rebuilt.

.. _googletest-submodule:

The GoogleTest submodule
~~~~~~~~~~~~~~~~~~~~~~~~

GoogleTest is a **git submodule** at ``thirdparty/googletest``, pinned to a
specific commit. ``.gitmodules`` only records where the submodule lives and
where to fetch it from — a plain ``git clone`` leaves the directory empty.

The Makefile handles this for you. The test object rule depends (order-only)
on ``thirdparty/googletest/googletest/include/gtest/gtest.h``; if that header
is missing, Make runs:

.. code-block:: sh

   git submodule update --init --recursive

GoogleTest and GoogleMock are compiled straight from their single-file
amalgamations (``gtest-all.cc``, ``gmock-all.cc``, ``gtest_main.cc``). There is
no CMake step and nothing is installed system-wide.

Running
-------

.. code-block:: sh

   ./build/obj/relay_calculator

See :ref:`user-guide` for a walkthrough of the prompts and the report.

Running the tests
-----------------

.. code-block:: sh

   make -C build test

Run a single suite or test with a GoogleTest filter:

.. code-block:: sh

   ./build/obj/run_tests --gtest_filter='ConstellationCalculator.*'
   ./build/obj/run_tests --gtest_filter='*Sync*'

See :ref:`testing` for what each suite covers.

Cleaning
--------

``make -C build clean`` removes only build outputs. The repository-level
``clean.sh`` script additionally removes stray ``*.o`` files, ``*.dSYM``
bundles, and ``.claude/`` directories, skipping ``.git`` and ``thirdparty``:

.. code-block:: sh

   ./clean.sh

.. _building-docs:

Building this documentation
---------------------------

The documentation is a Sphinx project in ``docs/`` with PlantUML diagrams.

.. code-block:: sh

   make -C build html     # or, equivalently:
   make -C docs html

Output goes to ``docs/_build/html/index.html``.

Dependencies
~~~~~~~~~~~~

``make html`` checks — and where possible fetches — everything it needs before
running Sphinx:

.. list-table::
   :header-rows: 1
   :widths: 25 35 40

   * - Dependency
     - Check
     - If missing
   * - Python 3.10+
     - ``python3`` is on ``PATH`` and reports 3.10 or newer
     - Stops with an error. Use ``PYTHON=/path/to/python3`` to pick another
       interpreter.
   * - PlantUML
     - ``plantuml`` is on ``PATH`` and ``plantuml -version`` runs
     - Runs ``brew install plantuml`` when Homebrew is available (it brings
       its own Java runtime and Graphviz). Otherwise stops and prints the
       install command for macOS, Debian/Ubuntu, and Fedora.
   * - Sphinx, ``sphinx-rtd-theme``, ``sphinxcontrib-plantuml``
     - ``docs/.venv`` exists and is newer than ``docs/requirements.txt``
     - Creates ``docs/.venv``, upgrades ``pip``, and installs
       ``docs/requirements.txt``. Editing the requirements file triggers a
       reinstall on the next build.

Run ``make -C docs check-deps`` to perform only the checks.

.. list-table:: Documentation make targets
   :header-rows: 1
   :widths: 20 80

   * - Target
     - Description
   * - ``html`` *(default)*
     - Builds the HTML site. Warnings are treated as errors (``-W``), so a
       broken cross-reference or diagram fails the build.
   * - ``check-deps``
     - Checks Python and PlantUML (installing PlantUML with Homebrew if
       needed) without building.
   * - ``venv``
     - Creates/updates ``docs/.venv`` only.
   * - ``linkcheck``
     - Checks external links.
   * - ``clean``
     - Deletes ``docs/_build``.
   * - ``distclean``
     - Deletes ``docs/_build`` and ``docs/.venv``.

Troubleshooting
---------------

.. list-table::
   :header-rows: 1
   :widths: 40 60

   * - Symptom
     - Cause / fix
   * - ``fatal error: 'gtest/gtest.h' file not found``
     - The submodule is not checked out and the automatic fetch failed (for
       example, no network). Run ``git submodule update --init --recursive``.
   * - ``git submodule status`` shows a leading ``-``
     - Submodule registered but not initialised — same fix as above.
   * - ``no member named 'numbers' in namespace 'std'``
     - Compiler too old or not in C++20 mode. Use a newer compiler via
       ``CXX=``.
   * - Editing a header does not trigger a rebuild
     - The Makefile does not track header dependencies. Run
       ``make -C build clean`` and rebuild.
   * - Docs build: ``plantuml: command not found``
     - Install PlantUML and make sure it is on ``PATH``.
   * - Docs build: ``Unable to locate a Java Runtime``
     - The system ``java`` stub has no JDK behind it. Use Homebrew's
       ``plantuml`` wrapper, which points at its own OpenJDK.
