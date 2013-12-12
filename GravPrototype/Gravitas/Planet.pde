class Planet
{
  PVector pos;
  PVector vel;
  PVector accel;
  //PVector rotVel;
  float mass;
  float radius;
  float fallOffDist;
  String type;

  LinkedList pastPts;
  //List of PVectors to represent future trajectory

  Planet(float radius, PVector pos, PVector vel, String type)
  {
    this.radius = radius;
    this.type = type;
    calcMass();
    fallOffDist = 0;
    this.pos = pos;
    this.vel = vel;
    accel = new PVector(0, 0, 0);

    pastPts = new LinkedList();
  }
  
  void calcMass()
  {
    float massScalar;
    if(type == SUN)           // immovable, large mass ( SUNS, MANNNNNN )
    {
      massScalar = 1;
    }
    else if(type == PLANET)   // moving heavy ( realistic, influential celestial bodies in a system )
    {
      massScalar = .1f;
    }
    else if(type == ASTEROID) // moving very low mass, so low that dont have noticeable gravitational influence ( good for aesthetics/particles/moons )
    {
      massScalar = .01f;
    }
    else
    {
      massScalar = 999999;
    }
    this.mass = (float)(massScalar*((4f/3f)*PI*(radius*radius*radius))/10f); // Volume = (4f/3f)*PI*(radius*radius*radius)
  }
  
  void setOrbit(Planet myParent, float radius, PVector normal, float rotation, float semiMajLMult) // normal is axis of rotation, should also add pos or neg velocity
  {
    float unknownY = -normal.x/normal.y; // 0 = x1*x2 + y1*unknownY + z1*z2
                                         // 0 = x1 + y1*unknownY
                                         // unknownY = -x1/y1
    if(normal.y == 0)
    {unknownY = 0;}
    PVector alongX = new PVector(1,unknownY,0); // is orthoganal to normal
    PVector tempPos;
    if(PVector.angleBetween(normal,alongX) != 0)
    {
      tempPos = normal.cross(alongX);     // pos from parent
    }
    else
    {
      tempPos = new PVector(0,1,0);
    }
    tempPos.normalize();
    tempPos.mult(radius);                // scale to appropriate dist out
    // rotate about normal, use 3D Rotation Matrix
    pos = PVector.add(myParent.pos, tempPos);   // place in world!
    println("tempPos.mag: "+tempPos.mag()+", tempPos: " +tempPos+ ", actPos: " + pos);
    
    if(semiMajLMult == 0)
    {
      semiMajLMult = .00001;
    }
    float semiMajL = radius*semiMajLMult; //elliptical, where (dist from sun(foci) to planet at 0 degrees)*4 = semi-minor axis
    //semiMajL = radius; //circular
    //semiMajL = radius*99999999; //parabolic
    //semiMajL = radius*-2f; // for hyperbola, except will start at y-int 
    
    float velMag = (float)(Math.sqrt((myParent.mass*myParent.mass*G) * ((2/((myParent.mass + mass)*radius)) - (1/((myParent.mass+mass)*semiMajL))) )); // For any orbit
    PVector velDir = normal.cross(tempPos);  // orthoganal to pos from origin and normal from origin
    velDir.normalize();
    velDir.mult(velMag);  // scale to calc'd value
    vel = velDir;
    vel.add(myParent.vel);
  }
  
  void update()
  {
    if(type != SUN)
    {
    accel = new PVector(0, 0, 0);
    accel.add(calcGravity());
    //bounding();

    vel.add(accel);
    pos.add(vel);

    updateTrail();
    }
  }

  void display()
  {
    fill(255);
    noStroke();

    pushMatrix();
    translate(pos.x, pos.y, pos.z);
    sphereDetail(30); //defualt is 30, change for performance
    sphere(radius);
    popMatrix();
    
    displayTrail();
  }
  
  int currFrame = 0;
  void updateTrail()
  {
    currFrame++;
    if(currFrame % 6 == 0)
    {
    pastPts.addLast(new PVector(pos.x, pos.y, pos.z));
    }
    if (pastPts.size() > 1000)
    {
      pastPts.removeFirst();
    }
  }

  void displayTrail()
  {
    if (pastPts.size() > 0)
    {
      stroke(0);
      strokeWeight(1);
      PVector curr = (PVector)pastPts.get(0);
      PVector prev = (PVector)pastPts.get(0);
      for (int i = 0; i < pastPts.size(); i++)
      {
        prev = curr;
        curr = (PVector)pastPts.get(i);
        line(prev.x, prev.y, prev.z, curr.x, curr.y, curr.z);
      }
      curr = (PVector)pastPts.get(pastPts.size()-1);
      line(curr.x, curr.y, curr.z, pos.x, pos.y, pos.z);
    }
  }

  PVector calcGravity()
  {
    PVector totalG = new PVector(0, 0, 0);
    for (int i = 0; i < bodies.size(); i++)
    {
      //GRAVITY!!!
      Planet curr = bodies.get(i);
      if (this != curr) // no gravity between self and self
      {
        float distBetween = PVector.dist(this.pos, curr.pos);
        
        if (distBetween > .01)//this.radius + curr.radius) // else intersecting
        {
          float force = G*((this.mass*curr.mass)/(distBetween*distBetween));
          float gravAccel = force/mass;
          PVector toCurr = PVector.sub(curr.pos, this.pos);
          toCurr.normalize();
          toCurr.mult(gravAccel);
          totalG.add(toCurr);
        }
      }
    }
    return totalG;
  }
  
  void bounding()
  {
    Boolean out = false;
    float strength = 100f;
    if (pos.x + radius > worldSize.x/2f)
    {
      accel.add(new PVector(-(pos.x + radius - worldSize.x/2f)/strength, 0, 0));
      out = true;
    }
    else if (pos.x - radius < -worldSize.x/2f)
    {
      accel.add(new PVector(-(pos.x - radius + worldSize.x/2f)/strength, 0, 0));
      out = true;
    }

    if (pos.z + radius > worldSize.z/2f)
    {
      accel.add(new PVector(0, 0, -(pos.z + radius - worldSize.z/2f)/strength));
      out = true;
    }
    else if (pos.z - radius < -worldSize.z/2f)
    {
      accel.add(new PVector(0, 0, -(pos.z - radius + worldSize.z/2f)/strength));
      out = true;
    }

    if (pos.y + radius > worldSize.y/2f)
    {
      accel.add(new PVector(0, -(pos.y + radius - worldSize.y/2f)/strength, 0));
      out = true;
    }
    else if (pos.y - radius < -worldSize.y/2f)
    {
      accel.add(new PVector(0, -(pos.y - radius + worldSize.y/2f)/strength, 0));
      out = true;
    }
    
    if(out)
    {
      vel.mult(.98);
    }
  }
}
