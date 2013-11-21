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
    if(type == SUN)    
    {
      massScalar = 10;
    }
    else if(type == PLANET)
    {
      massScalar = 1;
    }
    else if(type == ASTEROID)
    {
      massScalar = .1;
    }
    else
    {
      massScalar = 999999;
    }
    this.mass = massScalar*((4f/3f)*PI*(radius*radius*radius))/10f; // Volume = (4f/3f)*PI*(radius*radius*radius)
  }
  void update()
  {
    if(type != "")
    {
    accel = new PVector(0, 0, 0);
    accel.add(calcGravity());
      bounding();

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

  void updateTrail()
  {
    pastPts.addLast(new PVector(pos.x, pos.y, pos.z));
    if (pastPts.size() > 30)
    {
      pastPts.removeFirst();
    }
  }

  void displayTrail()
  {
    if (pastPts.size() > 0)
    {
      println(pastPts.size());
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

        if (distBetween > this.radius + curr.radius) // else intersecting
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
  
  void setOrbit(Planet myParent, float radius)//, PVector normal) // normal is on axis of rotation
  {
    pos = PVector.add(myParent.pos, new PVector(radius,0,0));
    float dist = PVector.dist(pos, myParent.pos);
    dist = radius;
    float velMag = (float)(Math.sqrt((myParent.mass*myParent.mass*G)/((myParent.mass + mass)*dist)));
    //PVector initVel = PVector.
    vel = new PVector(0,0, velMag);
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
