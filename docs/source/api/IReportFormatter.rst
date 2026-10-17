IReportFormatter
================

:Header: ``hpp/IReportFormatter.hpp``
:Kind: interface (abstract class)
:Implemented by: :cpp:class:`ConsoleReportFormatter`
:Used by: :cpp:class:`Application`

.. uml:: ../diagrams/classes/IReportFormatter.puml
   :caption: IReportFormatter and its collaborators

.. cpp:class:: IReportFormatter

   Presents an :cpp:class:`OrbitalResult` to the user. The interface says
   nothing about the destination; the console implementation writes to
   stdout, but a JSON, CSV, or file writer would implement the same method.

   .. cpp:function:: virtual ~IReportFormatter() = default

   .. cpp:function:: virtual void print(const OrbitalResult &result) const = 0

      Presents one result.

      :param result: a result produced by
                     :cpp:func:`IConstellationCalculator::calculate`

      Implementations should not modify program state other than their output
      destination.
