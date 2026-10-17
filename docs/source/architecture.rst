.. _architecture:

Architecture
============

The calculator is a small program, but it is deliberately structured like a
larger one: every responsibility lives behind an interface, concrete classes
are wired together in a single place, and the domain logic is free of I/O so
that it can be tested in isolation.

Source layout
-------------

.. code-block:: text

   KSP_RelayOrbiter_calculator/
   ├── build/
   │   ├── Makefile                     Build, test, and clean targets
   │   └── obj/                         Build outputs (git-ignored)
   ├── docs/                            This documentation (Sphinx + PlantUML)
   ├── hpp/                             Headers — one per type
   ├── src/                             Implementations + main.cpp
   ├── utest/                           GoogleTest suites + TestFixtures.hpp
   ├── thirdparty/googletest/           GoogleTest git submodule
   └── clean.sh                         Removes build artefacts

.. list-table:: Header → source mapping
   :header-rows: 1
   :widths: 35 35 30

   * - Header (``hpp/``)
     - Source (``src/``)
     - Kind
   * - ``CelestialBody.hpp``
     - —
     - value type
   * - ``OrbitalResult.hpp``
     - —
     - value type
   * - ``IOrbitalMechanics.hpp``
     - —
     - interface
   * - ``IBodyDatabase.hpp``
     - —
     - interface
   * - ``IConstellationCalculator.hpp``
     - —
     - interface
   * - ``IReportFormatter.hpp``
     - —
     - interface
   * - ``ITimeFormatter.hpp``
     - —
     - interface
   * - ``OrbitalMechanics.hpp``
     - ``OrbitalMechanics.cpp``
     - implementation
   * - ``KspBodyDatabase.hpp``
     - ``KspBodyDatabase.cpp``
     - implementation
   * - ``ConstellationCalculator.hpp``
     - ``ConstellationCalculator.cpp``
     - implementation
   * - ``ConsoleReportFormatter.hpp``
     - ``ConsoleReportFormatter.cpp``
     - implementation
   * - ``KerbinTimeFormatter.hpp``
     - ``KerbinTimeFormatter.cpp``
     - implementation
   * - ``Application.hpp``
     - ``Application.cpp``
     - orchestrator
   * - —
     - ``main.cpp``
     - composition root

Layers
------

.. uml:: diagrams/components.puml
   :caption: Components and their dependencies

.. list-table::
   :header-rows: 1
   :widths: 20 35 45

   * - Layer
     - Members
     - Responsibility
   * - Composition root
     - ``main.cpp``
     - Creates every concrete object and injects it. The only file that names
       concrete types.
   * - Presentation
     - :cpp:class:`Application`, :cpp:class:`ConsoleReportFormatter`,
       :cpp:class:`KerbinTimeFormatter`
     - All terminal input and output.
   * - Domain
     - :cpp:class:`ConstellationCalculator`, :cpp:class:`OrbitalMechanics`
     - The physics. Pure functions — no I/O, no mutable state.
   * - Data
     - :cpp:class:`KspBodyDatabase`
     - The read-only body catalogue.
   * - Value types
     - :cpp:class:`CelestialBody`, :cpp:class:`OrbitalResult`
     - Plain structs passed between layers.

Dependencies point inward: presentation depends on domain and data through
interfaces; domain depends only on value types and on
:cpp:class:`IOrbitalMechanics`; nothing depends on presentation.

Class model
-----------

.. uml:: diagrams/class_overview.puml
   :caption: All classes, interfaces, and relationships

Each interface has exactly one production implementation:

.. list-table::
   :header-rows: 1

   * - Interface
     - Implementation
     - Depends on
   * - :cpp:class:`IOrbitalMechanics`
     - :cpp:class:`OrbitalMechanics`
     - —
   * - :cpp:class:`IBodyDatabase`
     - :cpp:class:`KspBodyDatabase`
     - —
   * - :cpp:class:`IConstellationCalculator`
     - :cpp:class:`ConstellationCalculator`
     - :cpp:class:`IOrbitalMechanics`
   * - :cpp:class:`IReportFormatter`
     - :cpp:class:`ConsoleReportFormatter`
     - :cpp:class:`ITimeFormatter`
   * - :cpp:class:`ITimeFormatter`
     - :cpp:class:`KerbinTimeFormatter`
     - —

All implementations are declared ``final``: extension happens by implementing
the interface again, not by inheriting from an implementation.

Composition root
----------------

``main()`` builds the object graph bottom-up and hands it to
:cpp:class:`Application`:

.. code-block:: cpp

   int main() {
       auto db        = std::make_shared<KspBodyDatabase>();
       auto mechanics = std::make_shared<OrbitalMechanics>();
       auto calculator= std::make_shared<ConstellationCalculator>(mechanics);
       auto time_fmt  = std::make_shared<KerbinTimeFormatter>();
       auto formatter = std::make_shared<ConsoleReportFormatter>(time_fmt);

       Application app(db, calculator, formatter, time_fmt);
       app.run();
       return 0;
   }

.. uml:: diagrams/composition_root.puml
   :caption: Runtime object graph

Ownership is shared (``std::shared_ptr``) because one object can be used by
several others — the :cpp:class:`KerbinTimeFormatter` instance is held by both
the formatter and the application. All objects live for the whole run and are
released when ``main`` returns.

Runtime behaviour
-----------------

A complete session, from banner to exit:

.. uml:: diagrams/seq_session.puml
   :caption: Sequence of one session

The core calculation in detail:

.. uml:: diagrams/seq_calculate.puml
   :caption: Inside ConstellationCalculator::calculate()

Design principles
-----------------

Single Responsibility
   Physics (:cpp:class:`OrbitalMechanics`), data
   (:cpp:class:`KspBodyDatabase`), derivation
   (:cpp:class:`ConstellationCalculator`), output
   (:cpp:class:`ConsoleReportFormatter`), time formatting
   (:cpp:class:`KerbinTimeFormatter`), and interaction flow
   (:cpp:class:`Application`) each have their own class.

Open/Closed
   New behaviour — another calendar, another output format, another body
   catalogue — is added as a new class implementing an existing interface.
   :cpp:class:`Application` does not change.

Liskov Substitution
   Callers only use the interface contract, so any conforming implementation
   can be substituted — for example a GoogleMock double in a future test.

Interface Segregation
   Interfaces are narrow: :cpp:class:`IBodyDatabase` is read-only,
   :cpp:class:`ITimeFormatter` has a single method, and
   :cpp:class:`IOrbitalMechanics` contains only physics.

Dependency Inversion
   Classes receive their collaborators as interface pointers through the
   constructor. Only ``main.cpp`` knows the concrete types — with one exception,
   noted below.

Data-driven design
   All body parameters live in one static table. The body list, SOI checks,
   synchronous-orbit tags, and child-body menu options are all derived from it,
   so there is no body-specific ``if`` anywhere in the code.

Pure domain functions
   :cpp:func:`IConstellationCalculator::calculate` and every
   :cpp:class:`IOrbitalMechanics` method are deterministic functions of their
   arguments. They are trivially unit-testable and reusable for a future batch
   mode.

Deviations from the design
--------------------------

The code is not perfectly aligned with its stated principles. These are
documented so that they are fixed deliberately rather than copied:

* :cpp:func:`Application::run` creates its own :cpp:class:`OrbitalMechanics`
  for the altitude menu instead of receiving an :cpp:class:`IOrbitalMechanics`
  through the constructor. This is the only concrete type instantiated outside
  ``main.cpp``.
* :cpp:member:`Application::time_formatter_` is injected but never used by
  :cpp:class:`Application` itself; only the report formatter uses a time
  formatter.

See :ref:`limitations` for the complete list.

Error-handling strategy
-----------------------

* **User input** is validated in :cpp:class:`Application`; invalid input
  prints a message and re-prompts. The domain layer assumes valid input.
* **Optional data** (no SOI, no parent, no atmosphere limit) is expressed with
  ``std::optional`` rather than sentinel values.
* **Exceptions** are not part of the design. The only exception paths are
  inside the standard library parsing functions; see :ref:`limitations` for
  the one case that is not caught.
