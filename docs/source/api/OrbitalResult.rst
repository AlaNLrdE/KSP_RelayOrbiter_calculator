OrbitalResult
=============

:Header: ``hpp/OrbitalResult.hpp``
:Kind: value object

.. uml:: ../diagrams/classes/OrbitalResult.puml
   :caption: OrbitalResult, its producer and its consumer

.. cpp:struct:: OrbitalResult

   Every value computed for one body and target altitude. Produced by
   :cpp:func:`IConstellationCalculator::calculate` and consumed by
   :cpp:func:`IReportFormatter::print`.

   All distances are in **metres**, times in **seconds**, and velocities in
   **m/s**. Numeric members default to ``0.0`` and flags to ``false``, so a
   default-constructed result is valid but meaningless.

   .. rubric:: Identity

   .. cpp:member:: std::string body_name

      Name of the body the result was computed for.

   .. cpp:member:: double h_target = 0.0

      Target circular orbit altitude above the surface — the input to the
      calculation.

   .. rubric:: Circular orbit (final satellite orbit)

   .. cpp:member:: double r_c = 0.0

      Circular orbit radius, :math:`R + h_{target}`.

   .. cpp:member:: double T_c = 0.0

      Circular orbit period, :math:`2\pi\sqrt{r_c^3/\mu}`.

   .. cpp:member:: double v_c = 0.0

      Circular orbital velocity, :math:`\sqrt{\mu / r_c}`.

   .. rubric:: Elliptical parking orbit (carrier)

   .. cpp:member:: double T_e = 0.0

      Parking orbit period, exactly :math:`\tfrac{2}{3} T_c`.

   .. cpp:member:: double a_e = 0.0

      Parking orbit semi-major axis, :math:`r_c (2/3)^{2/3}`.

   .. cpp:member:: double r_apo = 0.0

      Apoapsis radius. Always equal to :cpp:member:`r_c`.

   .. cpp:member:: double h_apo = 0.0

      Apoapsis altitude. Always equal to :cpp:member:`h_target`.

   .. cpp:member:: double r_peri = 0.0

      Periapsis radius, :math:`2 a_e - r_c \approx 0.52629\, r_c`.

   .. cpp:member:: double h_peri = 0.0

      Periapsis altitude, :math:`r_{peri} - R`. Negative when the periapsis is
      underground.

   .. cpp:member:: double v_apo = 0.0

      Carrier speed at apoapsis (vis-viva with :math:`r = r_c`,
      :math:`a = a_e`). Always less than :cpp:member:`v_c`.

   .. cpp:member:: double v_peri = 0.0

      Carrier speed at periapsis (vis-viva with :math:`r = r_{peri}`,
      :math:`a = a_e`). Always greater than :cpp:member:`v_apo`.

   .. rubric:: Safety

   .. cpp:member:: bool surface_ok = false

      ``true`` when :math:`h_{peri} > 0`.

   .. cpp:member:: bool atm_ok = false

      ``true`` when the body has no atmosphere, or :math:`h_{peri}` is above
      it.

   .. cpp:member:: double min_h_surface = 0.0

      Smallest target altitude whose periapsis clears the surface,
      :math:`R/f - R` with :math:`f = 2(2/3)^{2/3} - 1`. Always set.

      .. note::

         The header comment says "(km)", but the value is in metres like every
         other field. The report divides by 1000 when printing.

   .. cpp:member:: std::optional<double> min_h_atm

      Smallest target altitude whose periapsis clears the atmosphere,
      :math:`(R + h_{atm})/f - R`. ``std::nullopt`` for airless bodies.
      Metres, despite the "(km)" header comment.

Relationships between fields
----------------------------

For every result produced by :cpp:class:`ConstellationCalculator`:

.. math::

   r_{apo} = r_c, \quad h_{apo} = h_{target}, \quad
   \frac{T_e}{T_c} = \frac{2}{3}, \quad
   r_{peri} + r_{apo} = 2 a_e, \quad
   v_{peri} > v_c > v_{apo}

For a body **with** an atmosphere, :cpp:member:`OrbitalResult::atm_ok`
implies :cpp:member:`OrbitalResult::surface_ok`. For an airless body
:cpp:member:`OrbitalResult::atm_ok` is always ``true`` — even when the
periapsis is underground — so consumers must check
:cpp:member:`OrbitalResult::surface_ok` first, as
:cpp:class:`ConsoleReportFormatter` does.
