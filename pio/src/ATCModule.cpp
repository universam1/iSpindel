
#include "ATCModule.h"

float_t ATCModule::C2F(float_t c)
 {
    return (c * 1.8 + 32);
 }

double_t ATCModule::sgToPlato(double_t SG)
{ // function to convert a value in specific gravity as plato
  // equation based on HBD#3204 post by AJ DeLange
  double_t plato;
  plato = -616.989 + 1111.488*SG - 630.606*SG*SG + 136.10305*SG*SG*SG;
  return plato;
}

double_t ATCModule::platoToSG(double_t plato)
{  // function to convert a value in Plato to SG
   // equation based on HBD#3723 post by AJ DeLange

   double_t SG;
   SG = 1.0000131 + 0.00386777*plato + 1.27447E-5*plato*plato + 6.34964E-8*plato*plato*plato;
   return SG;
}

float_t ATCModule::getCorrectedGravity(float_t measured_gravity)
{
    float_t corrected_gravity = measured_gravity;

    if (my_atcFormulaType == ATC_FORMULA_INTERNAL)
    {
        corrected_gravity = applyInternalFormula(measured_gravity);
    }
    else
    {
        /* ATC_FORMULA_CUSTOM */
    }
    
    return corrected_gravity;
}

double_t ATCModule::applyInternalFormula(double_t measured_gravity)
{
    double_t corrected_gravity = measured_gravity;

    if (my_gravityUnits == GRAVITY_UNITS_PLATO)
    {
        corrected_gravity = platoToSG(corrected_gravity);
    }
    
    corrected_gravity = calcCorrectedSG(corrected_gravity);

    if (my_gravityUnits == GRAVITY_UNITS_PLATO)
    {
        corrected_gravity = sgToPlato(corrected_gravity);
    }

    return corrected_gravity;
}

double_t ATCModule::calcCorrectedSG(double_t sg)
{
    double_t temp_F = C2F(Temperatur);
    double_t calibrationTemp_F = C2F(my_atcCalibrationTemp);
    double_t corrected_sg = sg * ((1.00130346 - 0.000134722124 * temp_F + 0.00000204052596 * temp_F * temp_F - 0.00000000232820948 * temp_F * temp_F * temp_F) 
                                                                / 
                                                                    (1.00130346 - 0.000134722124 * calibrationTemp_F + 0.00000204052596 * calibrationTemp_F * calibrationTemp_F - 0.00000000232820948 * calibrationTemp_F * calibrationTemp_F * calibrationTemp_F));

    //CONSOLELN(String("Internal atc formula used. Measured G: ") + sg + "   corrected G:" + corrected_sg);
    /*
    Serial.print("Internal atc formula used. Measured G: ");
    Serial.print(sg, 6);
    Serial.print("   corrected G:");
    Serial.print(corrected_sg, 6);
    Serial.print("\n");
    */

    return corrected_sg;
}
