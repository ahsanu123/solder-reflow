
// COPIED FROM
// https://github.com/PaulStoffregen/Encoder/blob/c083e9cbd6400f7e72a794c0d371a00a09d2a25d/Encoder.h#L162
//
//                           _______         _______
//               Pin1 ______|       |_______|       |______ Pin1
// negative <---         _______         _______         __      --> positive
//               Pin2 __|       |_______|       |_______|   Pin2

//	new	new	old	old
//	pin2	pin1	pin2	pin1	Result
//	----	----	----	----	------
//	0	0	0	0	no movement
//	0	0	0	1	+1
//	0	0	1	0	-1
//	0	0	1	1	+2  (assume pin1 edges only)
//	0	1	0	0	-1
//	0	1	0	1	no movement
//	0	1	1	0	-2  (assume pin1 edges only)
//	0	1	1	1	+1
//	1	0	0	0	+1
//	1	0	0	1	-2  (assume pin1 edges only)
//	1	0	1	0	no movement
//	1	0	1	1	-1
//	1	1	0	0	+2  (assume pin1 edges only)
//	1	1	0	1	-1
//	1	1	1	0	+1
//	1	1	1	1	no movement
/*
        // Simple, easy-to-read "documentation" version :-)
        //
        void update(void) {
                uint8_t s = state & 3;
                if (digitalRead(pin1)) s |= 4;
                if (digitalRead(pin2)) s |= 8;
                switch (s) {
                        case 0: case 5: case 10: case 15:
                                break;
                        case 1: case 7: case 8: case 14:
                                position++; break;
                        case 2: case 4: case 11: case 13:
                                position--; break;
                        case 3: case 12:
                                position += 2; break;
                        default:
                                position -= 2; break;
                }
                state = (s >> 2);
        }
*/

#include "v2/gui/menu.h"
#include <cstdint>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif
class Encoder {

  float _target;
  uint8_t _oldState = 0x00;
  uint8_t _oldSwitchState = 0x00;

public:
  float position = 0;
  float step = 1;
  float *connectedVariable;
  LcdGfxMenu *connectedMenu;
  bool switchState = false;

  float setToTarget(float *target);
  float decodeEncoder();
  bool decodeSwitch(float &target);

  Encoder();
};

#ifdef __cplusplus
}
#endif
