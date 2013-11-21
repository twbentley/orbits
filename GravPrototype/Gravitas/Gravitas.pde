import java.util.LinkedList;
import peasy.PeasyCam;

PeasyCam cam;

//List of planets
ArrayList<Planet> bodies;

float G = .1; // gravitational constant
Planet p1;
Planet theSun;
String SUN = "SUN";
String PLANET = "PLANET";
String ASTEROID = "ASTEROID";


PVector worldSize;

// LOL bad commenting srry will fix l8r lol, sorry, sorrynotsorry
void setup()
{
  size(1000,1000, P3D); // Right-handed coords
  frameRate(60);
  worldSize = new PVector(4500,4500,4500);
  bodies = new ArrayList<Planet>();
  
  //twoOrbitInit();
  ranParticleInit();
  //properOrbitInit();
  
  cam = new PeasyCam(this, 4500);
  cam.setMinimumDistance(50);
  cam.setMaximumDistance(4500);
}

void  draw()
{
  background(255/2f); // grey
  
  hint(ENABLE_DEPTH_TEST);
  hint(ENABLE_DEPTH_SORT);
  lights();
  for(int i = 0; i < bodies.size(); i++)
  {
    Planet curr = bodies.get(i);
    curr.update();
    curr.display();
  }
  drawMarkers();
}

void properOrbitInit()
{
  p1 = new Planet(100, new PVector(0, 0, 0), new PVector(0,0,0), SUN);
  bodies.add(p1);
  theSun = p1;
  
  p1 = new Planet(20, new PVector(0, 0, 0), new PVector(0,0,0), PLANET);
  bodies.add(p1);
  p1.setOrbit(theSun, 1000);
  
  p1 = new Planet(10, new PVector(0, 0, 0), new PVector(0,0,0), ASTEROID);
  bodies.add(p1);
  p1.setOrbit(bodies.get(1), 50);
  p1.vel = PVector.add(p1.vel, bodies.get(1).vel);
}
void twoOrbitInit()
{
  p1 = new Planet(20, new PVector(worldSize.z/4f, 0, 0), new PVector(0,5,0), PLANET);
  bodies.add(p1);
  p1 = new Planet(20, new PVector(-worldSize.z/4f, 0, 0), new PVector(0,-5,0), PLANET);
  bodies.add(p1);
  p1 = new Planet(90, new PVector(0,0,0), new PVector(0,0,0), SUN);
  bodies.add(p1);
}
void ranParticleInit()
{
  int numParticles = 6;
  for(int i = 0; i < numParticles; ++i)
  {
    p1 = new Planet(30, new PVector((float)Math.random()*worldSize.x/2f - worldSize.x/4f,(float)Math.random()*worldSize.y/2f - worldSize.y/4f,(float)Math.random()*worldSize.z/2f - worldSize.z/4f), 
                       new PVector((float)Math.random()*20f - 10,(float)Math.random()*20f - 10,(float)Math.random()*20f - 10),
                       ASTEROID);
    bodies.add(p1);
  }
  p1 = new Planet(200, new PVector(0,0,0), 
                       new PVector(5,0,0),
                       SUN);
    bodies.add(p1);
}

void drawMarkers()
{
  fill(255,125,125,255);
  //noStroke();
  
  // BACK 'FACE'
  pushMatrix();
  translate(-worldSize.x/2f,-worldSize.y/2f,-worldSize.z/2f);
  box(10);
  popMatrix();
  pushMatrix();
  translate(-worldSize.x/2f, worldSize.y/2f,-worldSize.z/2f);
  box(10);
  popMatrix();
  pushMatrix();
  translate( worldSize.x/2f,-worldSize.y/2f,-worldSize.z/2f);
  box(10);
  popMatrix();
  pushMatrix();
  translate( worldSize.x/2f, worldSize.y/2f,-worldSize.z/2f);
  box(10);
  popMatrix();
  
  // FRONT 'FACE'
  pushMatrix();
  translate(-worldSize.x/2f,-worldSize.y/2f, worldSize.z/2f);
  box(10);
  popMatrix();
  pushMatrix();
  translate(-worldSize.x/2f, worldSize.y/2f, worldSize.z/2f);
  box(10);
  popMatrix();
  pushMatrix();
  translate( worldSize.x/2f,-worldSize.y/2f, worldSize.z/2f);
  box(10);
  popMatrix();
  pushMatrix();
  translate( worldSize.x/2f, worldSize.y/2f, worldSize.z/2f);
  box(10);
  popMatrix();
  
  strokeWeight(1);
  stroke(0);
  line( worldSize.x/2f, worldSize.y/2f, worldSize.z/2f, -worldSize.x/2f, worldSize.y/2f, worldSize.z/2f);
  line( worldSize.x/2f, worldSize.y/2f, worldSize.z/2f,  worldSize.x/2f,-worldSize.y/2f, worldSize.z/2f);
  line(-worldSize.x/2f,-worldSize.y/2f, worldSize.z/2f, -worldSize.x/2f, worldSize.y/2f, worldSize.z/2f);
  line(-worldSize.x/2f,-worldSize.y/2f, worldSize.z/2f,  worldSize.x/2f,-worldSize.y/2f, worldSize.z/2f);
  
  line( worldSize.x/2f, worldSize.y/2f,-worldSize.z/2f, -worldSize.x/2f, worldSize.y/2f,-worldSize.z/2f);
  line( worldSize.x/2f, worldSize.y/2f,-worldSize.z/2f,  worldSize.x/2f,-worldSize.y/2f,-worldSize.z/2f);
  line(-worldSize.x/2f,-worldSize.y/2f,-worldSize.z/2f, -worldSize.x/2f, worldSize.y/2f,-worldSize.z/2f);
  line(-worldSize.x/2f,-worldSize.y/2f,-worldSize.z/2f,  worldSize.x/2f,-worldSize.y/2f,-worldSize.z/2f);
  
  line( worldSize.x/2f, worldSize.y/2f, worldSize.z/2f,  worldSize.x/2f, worldSize.y/2f,-worldSize.z/2f);
  line(-worldSize.x/2f, worldSize.y/2f, worldSize.z/2f, -worldSize.x/2f, worldSize.y/2f,-worldSize.z/2f);
  line( worldSize.x/2f,-worldSize.y/2f, worldSize.z/2f,  worldSize.x/2f,-worldSize.y/2f,-worldSize.z/2f);
  line(-worldSize.x/2f,-worldSize.y/2f, worldSize.z/2f, -worldSize.x/2f,-worldSize.y/2f,-worldSize.z/2f);
}
