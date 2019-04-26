#ifndef ASV_DYNAMICS_H
#define ASV_DYNAMICS_H

#include <array>
#include "sea_surface_dynamics.h"
#include "geometry.h"

namespace asv_swarm
{
namespace Hydrodynamics
{
/**
 * A simple structure to contain all input data about ASV.
 */
struct ASV
{
public:
  Quantity<Units::length> L; // length on load water line
  Quantity<Units::length> B; // beam of ASV at midship
  Quantity<Units::length> T; // draft of ASV at midship
  Quantity<Units::volume> displacement; // displacement at load water line
  Geometry::Point centre_of_gravity; // Also the control point of ASV
  Quantity<Units::length> metacentric_height; // metacentric height from keel
  Quantity<Units::length> r_roll; // roll radius of gyration
  Quantity<Units::length> r_pitch; // pitch radius of gyration
  Quantity<Units::length> r_yaw; // yaw radius of gyration
  Quantity<Units::velocity> max_speed; // maximum operational speed of the ASV.
};

/**
 * Class for calculation the dynamics of ASV in waves, wind and current.
 */
class ASV_dynamics
{
public:
  /**
   * Constructor.
   * @param asv input parameters for ASV.
   * @param orientation of the ASV with respect to North direction. 
   * @param wave_spectrum for the sea state.
   */
  ASV_dynamics(ASV& asv, 
               Quantity<Units::plane_angle> orientation,
               Sea_surface_dynamics* sea_surface);


  /**
   * Set the position of the ASV in 3D space for the given instant of time.
   */
  void set_asv_attitude(Quantity<Units::time> current_time);

protected:
  /**
   * Set the wave for matrix for the current time step.
   */
  void set_wave_force_matrix();

  /**
   * Set the propeller force matrix for the current time step.
   */
  void set_propeller_force_matrix();

  /**
   * Set the water current force matrix for the current time step.
   */
  void set_current_force_matrix();

  /**
   * Set the wind force matrix for the current time step.
   */
  void set_wind_force_matrix();

private:
  /**
   * Method to calculate added mass and set set mass matrix.
   */
  void set_mass_matrix();

  /**
   * Method to calculate the damping matrix. 
   * This method is currently just a place holder and does not contain any
   * implementation. Damping, currently, is assumed as zero for the following
   * reasons:
   * - The length of the vessel is much smaller compared to wave length.
   * - The vessel speed is considered small.
   * - The vessel does not create any significant waves because of its small
   *   size and low speed.
   */
  void set_damping_matrix(){;}

  /**
   * Method to calculate the hydrostatic stiffness and set stiffness matrix.
   */
  void set_stiffness_matrix();

  /**
   * Method to calculate the wave force and moments for a range of frequencies
   * and heading angles. This method populates table F_regular_waves.
   */
  void set_unit_wave_force_spectrum();

  /**
   * Method to calculate the heave force, pitch moment and roll moment for a
   * regular wave of 1cm height.
   * @param frequency of the regular wave
   * @param angle heading of the ASV with respect to the wave.
   */
  std::array<double, 3> get_unit_wave_heave_pitch_roll_force(
                                Quantity<Units::frequency> frequency,
                                Quantity<Units::plane_angle> angle);
  /**
   * Method to calculate the surge force on the ASV due to a regular wave of
   * height 1 cm. Currently considered as 0 Newton.
   */
  double get_unit_wave_surge_force (Quantity<Units::frequency> frequency, 
                                       Quantity<Units::plane_angle> angle);
  /**
   * Method to calculate the sway force on the ASV due to regular wave of height
   * 1 cm. Currently considered as 0 Newton.
   */
  double get_unit_wave_sway_force(Quantity<Units::frequency> frequency,
                                     Quantity<Units::plane_angle> angle);
  /**
   * Method to calculate the yaw moment on the ASV due to regular wave of height
   * 1 cm. Currently considered as 0 Newton-meter.
   */
  double get_unit_wave_yaw_moment(Quantity<Units::frequency> frequency,
                                     Quantity<Units::plane_angle> angle);

private:
  Sea_surface_dynamics* sea_surface;
  Quantity<Units::frequency> min_encounter_frequency; 
  Quantity<Units::frequency> max_encounter_frequency;
  int encounter_freq_band_count; // Number of frequency bands in the RAO.
  int encounter_wave_direction_count; // Number of heading directions
  
  ASV& asv;
  
  Quantity<Units::time> current_time;
  Quantity<Units::plane_angle> orientation; // orientation of the ASV with
                                            // global coordinates.
  double M[6][6]{0.0}; // mass + added mass
  double C[6][6]{0.0}; // potential damping + viscous damping
  double K[6][6]{0.0}; // hydrostatic stiffness
  double F_unit_wave[361][101][6]{0.0}; // Wave force and moment for waves of 
                                      // unit height at 360 different angles 
                                      // W.R.T the ASV and 100 frequencies 
                                      // between min and max encounter 
                                      // frequencies. 
  double F_wave[6]{0.0}; // Wave force matrix
  double F_propulsion[6]{0.0};  // Propeller thrust force matrix
  double F_current[6]{0.0}; // Water current force matrix
  double F_wind[6]{0.0}; // wind force matrix
};

} //namespace Hydrodynamics

} //namespace asv_swarm

#endif // ASV_DYNAMICS_H
