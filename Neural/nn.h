#ifndef _NN_H_
#define _NN_H_

/*
   TITLE    : Neural Net 
   AUTHOR   : LEBAB
   DATE     : 25/11/95
   OBJECT   : combinaison between neural net and genetic algo to make
   the computer ride a car
   ACTUAL   : net definition completed
   BUGS     : not tested
   */

#include "Common.h"
using namespace Go4D;

class Neuron;

struct Connexion
{
  Neuron * neuron;
  real weight;
};

class Layer;

class Neuron
{
friend Layer;
  public :
  Neuron();
  ~Neuron();
  void SetConnexion(int); // set the number of connexions, don't connect
  void Process();
  void InitRandom(real amplitude);
  void InitConst(real c);
  void Mute(real, real);
  void Copy(const Neuron &);
  void Reproduction(const Neuron &, real); // sexual reproduction
  Connexion & operator [](int) const;
  real Median();
  void RetroPropagate(real di, real gradian);
friend ostream & operator << (ostream &, const Neuron &); 
  //binary write of weights
friend istream & operator >> (istream &, Neuron &);
  real output;
  real input;
  real di; // result of quadratical derivation

  private :
  int nbconnexions;
  Connexion * connexions; //not owner
  static real Transfer(real);
  static real TransferPrime(real);
};

class Layer
{
  public :
  Layer(int =0); //nb neurons auto created
  ~Layer();
  void Process();
  void Connect(const Layer &);
  void InitRandom(real amplitude);
  void InitConst(real c);
  Neuron & operator [] (int i) const;
  void Copy(const Layer &);
  void Mute(real, real);
  void Reproduction(const Layer &, real );
  real Median();
  void RetroOutput(real, const Layer &);
  void RetroHidden(real, Layer &);
  real QuadraticError(const Layer &ideal);
friend ostream & operator << (ostream &, const Layer & ); 
  // binary write of neurons
friend istream & operator >> (istream &, Layer & );
  int NbNeurons() const {return nbneurons;}

  private :
  int nbneurons;
  Neuron * neurons; //owner
};

struct LayerList
{
  Layer * layer;
  LayerList * next,  * prev;
};

class Net
{ // for retroprogpagation the user of the net choose the gradian
  public :
  Net(Layer &, Layer &);
  ~Net();
  void AddLayer(Layer &);
  void Process();
  void Mute(real, real); //percentage, amplitude
  void Copy(const Net &);
  void Reproduction(const Net &, real); //percentage of weight changed
  real Median(); // of the weight
  void RetroPropagate(real gradian, const Layer &ideal);
  real QuadraticError(const Layer &ideal);
friend ostream & operator << (ostream &, const Net & ); 
  // binary write of neurons
friend istream & operator >> (istream &, Net & );
  Layer * entries, * outputs; // not owner

  private :
  int nblayers;
  LayerList * root, * queue; // not owner
};

inline real Neuron::Transfer(real value)
{
  return tanh(value);
}

inline real Neuron::TransferPrime(real value)
{
  if (fabs(value)>100) return 0;
  return 1/sqr(cosh(value));
}

#endif

