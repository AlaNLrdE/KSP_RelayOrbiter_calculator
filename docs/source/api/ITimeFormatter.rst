ITimeFormatter
==============

:Header: ``hpp/ITimeFormatter.hpp``
:Kind: interface (abstract class)
:Implemented by: :cpp:class:`KerbinTimeFormatter`
:Used by: :cpp:class:`ConsoleReportFormatter` (also injected into
          :cpp:class:`Application`, which does not use it)

.. uml:: ../diagrams/classes/ITimeFormatter.puml
   :caption: ITimeFormatter and its collaborators

.. cpp:class:: ITimeFormatter

   Converts a duration in seconds into a human-readable string. Isolating
   this behind an interface lets the calendar change (for example to Earth
   time, which KSP also supports) without touching the report code.

   .. cpp:function:: virtual ~ITimeFormatter() = default

   .. cpp:function:: virtual std::string format(double seconds) const = 0

      :param seconds: a duration in seconds
      :returns: the formatted duration
