
#include "ConvertHelper.h"

ConvertHelper::ConvertHelper(std::vector<State> NFA, std::vector<char> inputCharacters,
  std::vector<int> finalStates, int numFStates, int aSize, int numStates, int initial)
{
  this->NFA = NFA;
  this->inputCharacters = inputCharacters;
  this->finalStates = finalStates;
  this->numNfaFinalStates = numFStates;
  this->alphabetSize = aSize;
  this->numStates = numStates;
  this->nfaInitial = initial;

}

ConvertHelper::~ConvertHelper()
{

}

void ConvertHelper::NFAtoDFA()
{

  std::vector<DFAState> DStates;
  int index, DStatenum = 2;

  DFAState first = DFAState(1, Eclosure(nfaInitial));
  DStates.push_back(first);

  std::cout<<"\nE-closure(IO) = ";
  printVector(first.myStates);
  std::cout<<" = 1\n\n";

  while(unmarkedState(DStates)) //while there is an unmarked state in Dstates
  {
    int index = getFirstUnmarked(DStates);
    std::cout<<"Mark "<<DStates[index].myNum<<"\n";
    DStates[index].DFAmark = true;
    DFAState T = DStates[index];

    for(int i=0; i<inputCharacters.size()-1; i++) // for each input character
    {

      std::vector<int> NFAmoves = charMovesSet(T.myStates, inputCharacters[i]);

      if(!NFAmoves.empty())
      {
        printVector(T.myStates);
        std::cout<<" --"<<inputCharacters[i]<<"--> ";
        printVector(NFAmoves);
        std::cout<<"\nE-closure";
        printVector(NFAmoves);
        std::vector<int> U = EclosureSet(NFAmoves);
        std::cout<<" = ";
        printVector(U);
        std::cout<<" = "<<DStatenum<<"\n\n";


        // std::cout<<"\n\nDoes DStates contain ";
        // printVector(U);
        // std::cout<<" ? : "<<DStatescontains(DStates, U)<<"\n\n";
        if(!DStatescontains(DStates, U))
        {
          DFAState s = DFAState(DStatenum, U);
          DStates.push_back(s);
          DStatenum++;
        }



      }

    }
    std::cout<<"\n";

  }


}

bool ConvertHelper::DStatescontains(std::vector<DFAState> D, std::vector<int> NFAstates)
{
  for(int i = 0; i<D.size(); i++)
  {
    if(D[i].myStates == NFAstates)
    {
      return true;
    }
  }
  return false;
}

bool ConvertHelper::unmarkedState(std::vector<DFAState> DStates)
{
  for(unsigned int i =0; i<DStates.size(); i++)
  {
    if(DStates[i].DFAmark == false)
    {
      return true;
    }
  }
  return false;
}

int ConvertHelper::getFirstUnmarked(std::vector<DFAState> DStates)
{
  for(unsigned int i = 0; i<DStates.size(); i++)
  {
    if(DStates[i].DFAmark == false)
    {
      return i;
    }
  }
}

std::vector<int> ConvertHelper::Eclosure(int state)
{
  std::vector<int> closureSet;
  bool added = true;

  State currentState = NFA[state-1];
  if(!vectorContains(closureSet, currentState.myStateNum)) //add the current state
  {
    closureSet.push_back(currentState.myStateNum);
  }
  std::vector<int> moves = currentState.getEmoves();
  //std::cout<<"E MOVES: ";
  for(unsigned int j=0; j<moves.size(); j++)
  {
    if(!vectorContains(closureSet, moves[j])) // add the moves of the current state
    {
      closureSet.push_back(moves[j]);
    }
  }
  while(added) // while ive added something to the return set
  {
    added = false;

    std::vector<std::vector<int>> totalMoves;
    for(unsigned int i=0; i < closureSet.size(); i++) // get the moves of all states currently added
    {
      totalMoves.push_back(NFA[closureSet[i] - 1].getEmoves());
    }

    for(unsigned int i=0; i <totalMoves.size(); i++)
    {
      for(unsigned int j=0; j<totalMoves[i].size(); j++) // for each of the moves of the states added
      {
        if(!vectorContains(closureSet, totalMoves[i][j])) // add the move if the state is not added
        {
          closureSet.push_back(totalMoves[i][j]);
          added = true; // a state was added, so the process must repeat
        }
      }
    }
  }

  std::sort(closureSet.begin(), closureSet.end());
  return closureSet;
}

std::vector<int> ConvertHelper::EclosureSet(std::vector<int> states)
{
  std::vector<int> closureSet; // = Eclosure(states[0]);
  std::vector<int> temp;

  for(unsigned int j=0; j<states.size(); j++)
  {
    temp = Eclosure(states[j]);
    closureSet = combine(closureSet, temp);
  }

  return closureSet;
}


std::vector<int> ConvertHelper::charMovesSet(std::vector<int> states, char a)
{
  std::vector<int> moveSet;
  std::vector<int> temp;
  for(unsigned int i =0; i<states.size(); i++)
  {
    State cur = NFA[states[i] - 1];
    temp = cur.getCharMoves(a);
    moveSet = combine(moveSet, temp);
  }

  return moveSet;


}

std::vector<int> ConvertHelper::combine(std::vector<int> first, std::vector<int> second)
{
  std::vector<int> result;
  while(!second.empty()||!first.empty())
  {
      if(first.empty())
      {
          result.insert(result.end(),second.begin(),second.end());
          return result;
      }
      if(second.empty())
      {
          result.insert(result.end(),first.begin(),first.end());
          return result;
      }
      if(first.front()==second.front())
      {
          result.push_back(first.front());
          first.erase(first.begin());
          second.erase(second.begin());
      }
      else if(first.front()<second.front())
      {
          result.push_back(first.front());
          first.erase(first.begin());
      }
      else
      {
          result.push_back(second.front());
          second.erase(second.begin());
      }
  }
  return result;

}

bool ConvertHelper::vectorContains(std::vector<int> v, int x)
{
  if(std::find(v.begin(), v.end(), x) != v.end())
  {
    return true;
  }
  return false;
}

std::vector<int> ConvertHelper::parseStateSet(std::string states)
{
  std::vector<int> transitions; //new int[std::count(states.begin(), states.end(), ',') + 1];

  if(states == "{}") {return transitions;}

  states.erase(states.begin());
  int i = 0, pos;
  std::string temp;

  while((states.find(",") != std::string::npos))
  {
    pos = states.find(",");
    temp = states.substr(0, pos);
    states.erase(0, pos+1);
    transitions.push_back(stoi(temp));
    i++;
  }
  states[states.find("}")] = 0;

  transitions.push_back(stoi(states));

  return transitions;

}

void ConvertHelper::printVector(std::vector<int> v)
  {
    std::cout<<"{";
    for(unsigned int i=0; i<v.size(); i++)
    {
      if(i != v.size()-1)
      {
        std::cout<<v[i]<<",";
      }
      else
      {
        std::cout<<v[i];
      }
    }
    std::cout<<"}";
  }

















  //
