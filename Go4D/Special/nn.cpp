/*
  TITLE    : Neural Net 
  AUTHOR   : LEBAB
  DATE     : 25/11/95
  OBJECT   : 
*/

#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include "NN.h"

/*
   ------------------------------------------------------------
   neuron
   ------------------------------------------------------------
   */

Neuron::Neuron()
{
  nbconnexions = 0;
  connexions = NULL;
  output = 0;
}

Neuron::~Neuron()
{  // delete the list of connections, but not the connected neurons
  if (connexions != NULL) delete connexions;
}

void Neuron::SetConnexion(int nb)
{
  if (connexions != NULL) delete connexions;
  nbconnexions = nb;
  connexions = new Connexion [nbconnexions];
  for (int i=0; i<nbconnexions; i++)
      connexions[i].weight += 0;
}

void Neuron::Process()
{
  input = 0;
  for (int i=0; i<nbconnexions; i++)  
    input += connexions[i].neuron->output * connexions[i].weight ;  
  output = Transfer(input);
}

void Neuron::InitRandom(real amplitude)
{
  for (int i=0; i<nbconnexions; i++)
    connexions[i].weight = (frand -0.5) * amplitude;    
}

void Neuron::InitConst(real c)
{
  for (int i=0; i<nbconnexions; i++)
    connexions[i].weight = c;    
}

void Neuron::Copy(const Neuron & neuron)
{
  assert(nbconnexions == neuron.nbconnexions);
  for (int i=0; i<nbconnexions; i++)
    connexions[i].weight = neuron.connexions[i].weight;
}

void Neuron::Reproduction(const Neuron & neuron, real p)
{ // combine the weight of the 2 neurons in the first with a percentage of p 
  assert(nbconnexions == neuron.nbconnexions);
  for (int i=0; i<nbconnexions; i++)
    if (frand < p) connexions[i].weight = neuron.connexions[i].weight;
}

void Neuron::Mute(real percent, real factor)
{
  real median = Median();
  for (int i=0; i<nbconnexions; i++)
    if ( frand <percent ) 
      connexions[i].weight += (frand-0.5)*factor*median;
}

Connexion & Neuron::operator [] (int i) const
{
  assert( i>=0 && i <nbconnexions );
  return connexions[i];
}

real Neuron::Median()
{
  real result = 0;
  for (int i=0; i<nbconnexions; i++)
    result += fabs(connexions[i].weight);
  result /= real(nbconnexions);
  return result;
}

void Neuron::RetroPropagate(real newdi, real gradian)
{
  real factor = newdi*gradian;
  di = newdi;
  for (int i=0; i<nbconnexions; i++)
    connexions[i].weight -= factor*connexions[i].neuron->output;
}

ostream & operator << (ostream & os, const Neuron & neuron)
{
  os.write((char *)&neuron.nbconnexions, sizeof(int) );
  for (int i=0; i<neuron.nbconnexions; i++)
    os.write((char *)&neuron.connexions[i].weight, sizeof(real) );
  return os;
}

istream & operator >> (istream & is, Neuron & neuron)
{
  int n;
  is.read( (char *)&n, sizeof(int) );
  assert (n == neuron.nbconnexions );
  for (int i=0; i<neuron.nbconnexions; i++)
    {
      real w;
      is.read((char *)&w, sizeof(real));
      neuron[i].weight = w;
    }
  return is;
}

/*
   ------------------------------------------------------------
   Layer
   ------------------------------------------------------------
   */

Layer::Layer(int nbneuron)
{
  nbneurons = nbneuron;
  neurons = new Neuron [nbneurons];
}

Layer::~Layer()
{ // delete the list and the neurons
  delete neurons;
}

void Layer::Process()
{ // process all the neurons of the list
  for (int i=0; i<nbneurons; i++)
    neurons[i].Process();
}

void Layer::Connect(const Layer &l)
{ // connect all the neurons of the layer with all the neuron of the other
  for (int i=0; i<nbneurons; i++)
    {
      neurons[i].SetConnexion(l.nbneurons);
      for (int j=0; j<l.nbneurons; j++)
	neurons[i][j].neuron = &l.neurons[j];
    }
}

void Layer::InitRandom(real amplitude)
{
  for (int i=0; i<nbneurons; i++)
    neurons[i].InitRandom(amplitude);
}

void Layer::InitConst(real c)
{
  for (int i=0; i<nbneurons; i++)
    neurons[i].InitConst(c);
}

Neuron & Layer::operator [](int i) const
{
  assert( i>=0 && i<nbneurons );
  return neurons[i];
}

void Layer::Mute(real percent, real factor)
{
  //real median = Median();
  for (int i=0; i<nbneurons; i++)
    neurons[i].Mute(percent, factor);
}

void Layer::Copy(const Layer & layer)
{
  assert(nbneurons==layer.nbneurons);
  for (int i=0; i<nbneurons; i++)
    neurons[i].Copy(layer[i]);
}

void Layer::Reproduction(const Layer & layer, real percent)
{
  assert(nbneurons==layer.nbneurons);
  for (int i=0; i<nbneurons; i++)
    neurons[i].Reproduction(layer[i], percent);  
} 

real Layer::Median()
{
  real result = 0;
  for (int i=0; i<nbneurons; i++)
    result += neurons[i].Median();\
  result /= real(nbneurons);
  return result;
}

#define Si neurons[i].output
#define Yi ideal[i].output
#define Ii neurons[i].input
#define dh nextlayer[h].di
#define Whi nextlayer[h].connexions[i].weight

void Layer::RetroOutput(real gradian, const Layer & ideal)
{
  assert( nbneurons == ideal.nbneurons );
  for (int i=0; i<nbneurons; i++)
    {
      //real di = 2*(Si-Yi)*Neuron::TransferPrime(Ii);
      real si = neurons[i].output;
      real yi = ideal[i].output;
      real ii = neurons[i].input;
      real ti = Neuron::TransferPrime(ii);
      real di = 2*(si-yi)*ti;
      neurons[i].RetroPropagate(di, gradian);
    }
}

void Layer::RetroHidden(real gradian, Layer &nextlayer)
{
  // assume that all the neurons of next layer are connected to all 
  // the neurons of the current layer
  //return ; //debug
  for (int i=0; i<nbneurons; i++)
    {
      real di = 0;
      real ii = neurons[i].input;
      real ti = Neuron::TransferPrime(ii);
      for (int h=0; h<nextlayer.nbneurons; h++)
	{
	  //di += dh*Whi*Neuron::TransferPrime(Ii);
	  real dhh = nextlayer[h].di;
	  real whi = nextlayer[h].connexions[i].weight;
	  di += dhh*whi*ti;
	}
      neurons[i].RetroPropagate(di, gradian);
    }
}

real Layer::QuadraticError(const Layer &ideal)
{
  assert( ideal.nbneurons == nbneurons );
  real result = 0;
  for (int i=0; i<nbneurons; i++)
    result += sqr(neurons[i].output-ideal[i].output);
  return result;
}

ostream & operator << (ostream & os, const Layer & layer)
{
  os.write((char *)&layer.nbneurons, sizeof(int) );
  for (int i=0; i<layer.nbneurons; i++)
    os << layer.neurons[i];
  return os;
}

istream & operator >> (istream & is, Layer & layer)
{
  int n;
  is.read( (char *)&n, sizeof(int));
  assert (n == layer.nbneurons );
  for (int i=0; i<layer.nbneurons; i++)
    is >> layer[i];
  return is;
}

/*
   ------------------------------------------------------------
   net
   ------------------------------------------------------------
   */

Net::Net(Layer &e, Layer &o)
{
  entries = &e; outputs = &o;
  root = NULL; queue=NULL;
  nblayers = 0;
}

Net::~Net()
{
  LayerList * next;
  for (LayerList * tmp=root; tmp != NULL; tmp=next)
    {
      next  = tmp->next;
      delete tmp;
    }
}

void Net::AddLayer(Layer & layer)
{
  LayerList * con = new LayerList;
  con->layer = &layer;
  con->next = NULL;
  if (root == NULL)
    root = con;
  else
    queue->next = con;
  con->prev = queue;
  queue = con;
  nblayers++;
}

void Net::Process()
{
  for (LayerList * tmp=root; tmp!=NULL; tmp=tmp->next)
    tmp->layer->Process();
  outputs->Process();
}

void Net::Mute(real percent, real factor)
{
  //real median = Median();
  for (LayerList * tmp=root; tmp!=NULL; tmp=tmp->next)
    tmp->layer->Mute(percent, factor);
  outputs->Mute(percent, factor);  
}

void Net::Reproduction(const Net & net, real percent)
{
  assert(nblayers == net.nblayers );
  for (LayerList * thistmp=root, * tmp=net.root; 
       thistmp!=NULL; thistmp=thistmp->next, tmp=tmp->next)
    thistmp->layer->Reproduction(*tmp->layer, percent);
  outputs->Reproduction(*net.outputs, percent);
}

void Net::Copy(const Net & net)
{
  assert(nblayers == net.nblayers );
  for (LayerList * thistmp=root, * tmp=net.root; 
       thistmp!=NULL; thistmp=thistmp->next, tmp=tmp->next)
    thistmp->layer->Copy(*tmp->layer);
  outputs->Copy(*net.outputs);
}

real Net::Median()
{
  real result=0;
  for (LayerList * tmp=root; tmp!=NULL;  tmp=tmp->next)
    result += tmp->layer->Median();
  result += outputs->Median();
  result /= real(nblayers+1);
  return result;
}

void Net::RetroPropagate(real gradian, const Layer & ideal)
{
  outputs->RetroOutput(gradian, ideal);
  if (queue != NULL)
    {
      queue->layer->RetroHidden(gradian, *outputs);
      LayerList * tmp = queue->prev;
      while (tmp != NULL)
        {
	  tmp->layer->RetroHidden(gradian, *tmp->next->layer);
	  tmp = tmp->prev;
	}
      entries->RetroHidden(gradian, *root->layer ) ;
    }
}

real Net::QuadraticError(const Layer &ideal)
{
  return outputs->QuadraticError(ideal);
}

ostream & operator << (ostream & os, const Net & net)
{
  os.write( (char *)&net.nblayers, sizeof(int));
  for (LayerList * tmp=net.root; tmp!=NULL;  tmp=tmp->next)
    os << *tmp->layer;
  os << *net.outputs;
  return os;
}

istream & operator >> (istream & is, Net & net)
{
  // don't modifie the old connexions, just read the new weight
  int n;
  is.read( (char *)&n, sizeof(int));
  assert(n == net.nblayers);
  for (LayerList * tmp=net.root; tmp!=NULL; tmp=tmp->next)
    is >> *tmp->layer;
  is >> *net.outputs;
  return is;
}

