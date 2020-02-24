#ifndef __ATCMODULE_H__
#define __ATCMODULE_H__

#include "Globals.h"
typedef enum tag_GRAVITY_UNITS
{
  GRAVITY_UNITS_SG,
  GRAVITY_UNITS_PLATO
} GRAVITY_UNITS;

typedef enum tag_ATC_FORMULA_TYPE
{
  ATC_FORMULA_INTERNAL,
  ATC_FORMULA_CUSTOM
} ATC_FORMULA_TYPE;

typedef enum tag_ATC_CUSTOM_OP_TYPE
{
  ATC_OPT_ADD,
  ATC_OPT_MUL,
  ATC_OPT_USEASIS
} ATC_CUSTOM_OP_TYPE;

#define ATC_CUSTOM_FORMULA_LENGTH 100

extern ATC_FORMULA_TYPE my_atcFormulaType;
extern GRAVITY_UNITS my_gravityUnits;
extern int16_t my_atcCalibrationTemp; // always in deg C, will be converted when/if needed
extern ATC_CUSTOM_OP_TYPE my_atcOpt;
extern char my_custom_atc_formula[ATC_CUSTOM_FORMULA_LENGTH];

extern float Temperatur; // actual temp reading

class ATCModule
{
    public:
        static float_t getCorrectedGravity(float_t);

    private:
        static double_t applyInternalFormula(double_t);
        static double_t calcCorrectedSG(double_t);

        static float_t C2F(float_t); 
        static double_t sgToPlato(double_t);
        static double_t platoToSG(double_t);

        // Disallow creating an instance of this object
        ATCModule() {}
};

#endif  //__ATCMODULE_H__
