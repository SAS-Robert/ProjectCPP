Original Values:

currentFrame.distanceTo(toFrame) <= 0.8
void setX_Resistance() {

  switch (this.resistance) {
  case 1:
    controlMode.parametrize(CartDOF.Y).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.X).setStiffness(5.0);
    controlMode.parametrize(CartDOF.Z).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.A).setStiffness(1);
    controlMode.parametrize(CartDOF.B).setStiffness(100);
    controlMode.parametrize(CartDOF.C).setStiffness(100);
    break;
  case 2:
    controlMode.parametrize(CartDOF.Y).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.X).setStiffness(15.0);
    controlMode.parametrize(CartDOF.Z).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.A).setStiffness(1);
    controlMode.parametrize(CartDOF.B).setStiffness(100);
    controlMode.parametrize(CartDOF.C).setStiffness(100);
    break;
  case 3:
    controlMode.parametrize(CartDOF.Y).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.X).setStiffness(25.0);
    controlMode.parametrize(CartDOF.Z).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.A).setStiffness(1);
    controlMode.parametrize(CartDOF.B).setStiffness(100);
    controlMode.parametrize(CartDOF.C).setStiffness(100);
    break;
  case 4:
    controlMode.parametrize(CartDOF.Y).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.X).setStiffness(35.0);
    controlMode.parametrize(CartDOF.Z).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.A).setStiffness(1);
    controlMode.parametrize(CartDOF.B).setStiffness(100);
    controlMode.parametrize(CartDOF.C).setStiffness(100);
    break;
  case 5:
    controlMode.parametrize(CartDOF.Y).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.X).setStiffness(45.0);
    controlMode.parametrize(CartDOF.Z).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.A).setStiffness(1);
    controlMode.parametrize(CartDOF.B).setStiffness(100);
    controlMode.parametrize(CartDOF.C).setStiffness(100);
    break;
  case 6:
    controlMode.parametrize(CartDOF.Y).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.X).setStiffness(55.0);
    controlMode.parametrize(CartDOF.Z).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.A).setStiffness(1);
    controlMode.parametrize(CartDOF.B).setStiffness(100);
    controlMode.parametrize(CartDOF.C).setStiffness(100);
    break;
  case 7:
    controlMode.parametrize(CartDOF.Y).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.X).setStiffness(65.0);
    controlMode.parametrize(CartDOF.Z).setStiffness(5000.0);
    // controlMode.parametrize(CartDOF.ROT).setStiffness(50);
    controlMode.parametrize(CartDOF.A).setStiffness(1);
    controlMode.parametrize(CartDOF.B).setStiffness(100);
    controlMode.parametrize(CartDOF.C).setStiffness(100);
    break;
  case 8:
    controlMode.parametrize(CartDOF.Y).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.X).setStiffness(75.0);
    controlMode.parametrize(CartDOF.Z).setStiffness(5000.0);
    // controlMode.parametrize(CartDOF.ROT).setStiffness(50);
    controlMode.parametrize(CartDOF.A).setStiffness(1);
    controlMode.parametrize(CartDOF.B).setStiffness(100);
    controlMode.parametrize(CartDOF.C).setStiffness(100);
    break;
  case 9:
    controlMode.parametrize(CartDOF.Y).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.X).setStiffness(85.0);
    controlMode.parametrize(CartDOF.Z).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.A).setStiffness(1);
    controlMode.parametrize(CartDOF.B).setStiffness(100);
    controlMode.parametrize(CartDOF.C).setStiffness(100);
    break;
  case 10:
    controlMode.parametrize(CartDOF.Y).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.X).setStiffness(95.0);
    controlMode.parametrize(CartDOF.Z).setStiffness(5000.0);
    controlMode.parametrize(CartDOF.A).setStiffness(1);
    controlMode.parametrize(CartDOF.B).setStiffness(100);
    controlMode.parametrize(CartDOF.C).setStiffness(100);
    break;
  }
}
