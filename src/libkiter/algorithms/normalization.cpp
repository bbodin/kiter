/*
 * normalization.cpp
 *
 *  Created on: 10 mars 2011
 *      Author: toky
 */

#include <models/Dataflow.h>
#include <algorithms/normalization.h>


using namespace models;


bool algorithms::check_validity (models::Dataflow *from, std::map<Vertex,TOKEN_UNIT> *  normalization ) {

    if (!normalization) return false;

    {ForEachEdge(from, c) {

        Vertex Ti = from->getEdgeSource(c);
        Vertex Tj = from->getEdgeTarget(c);

        TOKEN_UNIT Zi = normalization->at(Ti);
        TOKEN_UNIT Zj = normalization->at(Tj);

        TOKEN_UNIT Ui = from->getEdgeIn(c);
        TOKEN_UNIT Uj = from->getEdgeOut(c);

        {
        TOKEN_UNIT MOP = from->getPreload(c);
        TOKEN_UNIT gcda = from->getFineGCD(c);
        TOKEN_UNIT old  = commons::floor(MOP,gcda);
        // Here we know ALPHA = Zi/Ui = Zj/Uj
             TOKEN_FRACT result_i =  TOKEN_FRACT(old) * TOKEN_FRACT(Zi , Ui);
             TOKEN_FRACT result_j =  TOKEN_FRACT(old) * TOKEN_FRACT(Zj , Uj);

             TOKEN_UNIT new_result_i =  (old * Zi) / Ui;
             TOKEN_UNIT new_result_j =  (old * Zj) / Uj;

             if (result_i.denominator() != 1) return false;
             if (result_j.denominator() != 1) return false;


             if (result_i != new_result_i) {
                 VERBOSE_ERROR("TOKEN_UNIT result_i     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zi <<  "," << Ui << ") =" << result_i);
                 VERBOSE_ERROR("TOKEN_UNIT result_j     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zj <<  "," << Uj << ") =" << result_j);
                 VERBOSE_ERROR("TOKEN_UNIT new_result_i =(" << old << "*" <<  Zi <<  ") / " << Ui << "=" << new_result_i);
                 VERBOSE_ERROR("TOKEN_UNIT new_result_j =(" << old << "*" <<  Zj <<  ") / " << Uj << "=" << new_result_j);
                 VERBOSE_ERROR("DATA_UNIT OVERFLOW");
                 return false;

             }

             if (result_j != new_result_j) {
                 VERBOSE_ERROR("TOKEN_UNIT result_i     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zi <<  "," << Ui << ") =" << result_i);
                 VERBOSE_ERROR("TOKEN_UNIT result_j     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zj <<  "," << Uj << ") =" << result_j);
                 VERBOSE_ERROR("TOKEN_UNIT new_result_i =(" << old << "*" <<  Zi <<  ") / " << Ui << "=" << new_result_i);
                 VERBOSE_ERROR("TOKEN_UNIT new_result_j =(" << old << "*" <<  Zj <<  ") / " << Uj << "=" << new_result_j);
                 VERBOSE_ERROR("DATA_UNIT OVERFLOW");
                 return false;
             }

             if ( new_result_i != new_result_j) {
                 VERBOSE_ERROR("TOKEN_UNIT result_i     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zi <<  "," << Ui << ") =" << result_i);
                 VERBOSE_ERROR("TOKEN_UNIT result_j     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zj <<  "," << Uj << ") =" << result_j);
                 VERBOSE_ERROR("TOKEN_UNIT new_result_i =(" << old << "*" <<  Zi <<  ") / " << Ui << "=" << new_result_i);
                 VERBOSE_ERROR("TOKEN_UNIT new_result_j =(" << old << "*" <<  Zj <<  ") / " << Uj << "=" << new_result_j);
                 VERBOSE_ERROR("DATA_UNIT OVERFLOW");
                 return false;
                 //DATA_UNIT OVERFLOW
             }


             if (result_i.numerator()  !=  result_j.numerator() ) return false;
        }

        for (EXEC_COUNT  phase = 1 ; phase <= from->getPhasesQuantity(Ti) ; phase++ ) {

           TOKEN_UNIT old = from->getEdgeInPhase(c,phase);

            // Here we know ALPHA = Zi/Ui = Zj/Uj
           TOKEN_FRACT result_i =  TOKEN_FRACT(old) * TOKEN_FRACT(Zi , Ui);
           TOKEN_FRACT result_j =  TOKEN_FRACT(old) * TOKEN_FRACT(Zj , Uj);

           TOKEN_UNIT new_result_i =  (old * Zi) / Ui;
           TOKEN_UNIT new_result_j =  (old * Zj) / Uj;

           if (result_i.denominator() != 1) return false;
           if (result_j.denominator() != 1) return false;


           if (result_i != new_result_i) {
               VERBOSE_ERROR("TOKEN_UNIT result_i     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zi <<  "," << Ui << ") =" << result_i);
               VERBOSE_ERROR("TOKEN_UNIT result_j     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zj <<  "," << Uj << ") =" << result_j);
               VERBOSE_ERROR("TOKEN_UNIT new_result_i =(" << old << "*" <<  Zi <<  ") / " << Ui << "=" << new_result_i);
               VERBOSE_ERROR("TOKEN_UNIT new_result_j =(" << old << "*" <<  Zj <<  ") / " << Uj << "=" << new_result_j);
               VERBOSE_ERROR("DATA_UNIT OVERFLOW");
               return false;

           }

           if (result_j != new_result_j) {
               VERBOSE_ERROR("TOKEN_UNIT result_i     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zi <<  "," << Ui << ") =" << result_i);
               VERBOSE_ERROR("TOKEN_UNIT result_j     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zj <<  "," << Uj << ") =" << result_j);
               VERBOSE_ERROR("TOKEN_UNIT new_result_i =(" << old << "*" <<  Zi <<  ") / " << Ui << "=" << new_result_i);
               VERBOSE_ERROR("TOKEN_UNIT new_result_j =(" << old << "*" <<  Zj <<  ") / " << Uj << "=" << new_result_j);
               VERBOSE_ERROR("DATA_UNIT OVERFLOW");
               return false;
           }

           if ( new_result_i != new_result_j) {
               VERBOSE_ERROR("TOKEN_UNIT result_i     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zi <<  "," << Ui << ") =" << result_i);
               VERBOSE_ERROR("TOKEN_UNIT result_j     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zj <<  "," << Uj << ") =" << result_j);
               VERBOSE_ERROR("TOKEN_UNIT new_result_i =(" << old << "*" <<  Zi <<  ") / " << Ui << "=" << new_result_i);
               VERBOSE_ERROR("TOKEN_UNIT new_result_j =(" << old << "*" <<  Zj <<  ") / " << Uj << "=" << new_result_j);
               VERBOSE_ERROR("DATA_UNIT OVERFLOW");
               return false;
               //DATA_UNIT OVERFLOW
           }

           if (result_i.numerator()  !=  result_j.numerator() ) return false;

        }

        for (EXEC_COUNT  phase = 1 ; phase <= from->getPhasesQuantity(Tj) ; phase++ ) {
            TOKEN_UNIT old = from->getEdgeOutPhase(c,phase);

            // Here we know ALPHA = Zi/Ui = Zj/Uj
           TOKEN_FRACT result_i =  TOKEN_FRACT(old) * TOKEN_FRACT(Zi , Ui);
           TOKEN_FRACT result_j =  TOKEN_FRACT(old) * TOKEN_FRACT(Zj , Uj);

           TOKEN_UNIT new_result_i =  (old * Zi) / Ui;
           TOKEN_UNIT new_result_j =  (old * Zj) / Uj;

           if (result_i.denominator() != 1) return false;
           if (result_j.denominator() != 1) return false;


           if (result_i != new_result_i) {
               VERBOSE_WARNING("TOKEN_UNIT result_i     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zi <<  "," << Ui << ") =" << result_i);
               VERBOSE_WARNING("TOKEN_UNIT result_j     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zj <<  "," << Uj << ") =" << result_j);
               VERBOSE_WARNING("TOKEN_UNIT new_result_i =(" << old << "*" <<  Zi <<  ") / " << Ui << "=" << new_result_i);
               VERBOSE_WARNING("TOKEN_UNIT new_result_j =(" << old << "*" <<  Zj <<  ") / " << Uj << "=" << new_result_j);
               VERBOSE_WARNING("DATA_UNIT OVERFLOW");
               return false;

           }

           if (result_j != new_result_j) {
               VERBOSE_WARNING("TOKEN_UNIT result_i     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zi <<  "," << Ui << ") =" << result_i);
               VERBOSE_WARNING("TOKEN_UNIT result_j     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zj <<  "," << Uj << ") =" << result_j);
               VERBOSE_WARNING("TOKEN_UNIT new_result_i =(" << old << "*" <<  Zi <<  ") / " << Ui << "=" << new_result_i);
               VERBOSE_WARNING("TOKEN_UNIT new_result_j =(" << old << "*" <<  Zj <<  ") / " << Uj << "=" << new_result_j);
               VERBOSE_WARNING("DATA_UNIT OVERFLOW");
               return false;
           }

           if ( new_result_i != new_result_j) {
               VERBOSE_WARNING("TOKEN_UNIT result_i     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zi <<  "," << Ui << ") =" << result_i);
               VERBOSE_WARNING("TOKEN_UNIT result_j     = TOKEN_FRACT(" << old << ") * TOKEN_FRACT(" <<  Zj <<  "," << Uj << ") =" << result_j);
               VERBOSE_WARNING("TOKEN_UNIT new_result_i =(" << old << "*" <<  Zi <<  ") / " << Ui << "=" << new_result_i);
               VERBOSE_WARNING("TOKEN_UNIT new_result_j =(" << old << "*" <<  Zj <<  ") / " << Uj << "=" << new_result_j);
               VERBOSE_WARNING("DATA_UNIT OVERFLOW");
               return false;
               //DATA_UNIT OVERFLOW
           }


           if (result_i.numerator()  !=  result_j.numerator() ) return false;
        }

    }}

    return true;
}


bool algorithms::setNormalization(models::Dataflow *to, std::map<Vertex,TOKEN_UNIT> *  normalization) {
    VERBOSE_ASSERT(normalization,TXT_NEVER_HAPPEND);

    VERBOSE_INFO("Set normalization ...");

    {ForEachVertex(to, pVertex) {
        TOKEN_UNIT Z; try {Z = normalization->at(pVertex);} catch (...) {
            VERBOSE_ERROR("unexpected error in normalization function");
            return false;
        }
        VERBOSE_ASSERT(Z > 0, "Zi fail ?? impossible !");
        to->setZi(pVertex, Z);

        {ForConnectedEdges(to,pVertex,c) {
            Vertex Ti = to->getEdgeSource(c);
            Vertex Tj = to->getEdgeTarget(c);


             TOKEN_UNIT Zi = normalization->at(Ti);
             TOKEN_UNIT Zj = normalization->at(Tj);

             TOKEN_UNIT Ui = to->getEdgeIn(c);
             TOKEN_UNIT Uj = to->getEdgeOut(c);

            VERBOSE_ASSERT(Zi > 0,"Fail detetion for Fail normalization");
            VERBOSE_ASSERT(Ui > 0,"Fail detetion for Fail normalization");
            VERBOSE_ASSERT(Zj > 0,"Fail detetion for Fail normalization");
            VERBOSE_ASSERT(Uj > 0,"Fail detetion for Fail normalization");

             const TOKEN_FRACT result_i =  TOKEN_FRACT(Zi , Ui);
             const TOKEN_FRACT result_j =  TOKEN_FRACT(Zj , Uj);

             VERBOSE_ASSERT_EQUALS(result_i,result_j);

             VERBOSE_ASSERT_EQUALS((TOKEN_UNIT) result_i.numerator() , (TOKEN_UNIT) result_j.numerator());
             VERBOSE_ASSERT_EQUALS((TOKEN_UNIT) result_i.denominator() , (TOKEN_UNIT) result_j.denominator());

             to->setAlpha(c,result_i);
        }}

    }}
    VERBOSE_INFO("Done");
    return true;
}

std::map<Vertex,TOKEN_UNIT> * algorithms::rationalNormalize(models::Dataflow *from ) {

    if (from->getEdgesCount() <= 0 ) {

        std::map<Vertex,TOKEN_UNIT> * res = new  std::map<Vertex,TOKEN_UNIT>();
        {ForEachVertex(from,v) {
        	(*res)[v] = 1;
        }}
    	return res;
    }

    VERBOSE_INFO("Start Rational normalization...");

    std::map<Edge,TOKEN_FRACT> alphas; //! Liste des alphas (coefficient multiplicateur de normalisation)

    // ** Construction de la liste des tâches à voir
    std::set<Vertex> total_a_voir;
    std::set<Vertex> prochains_a_voir;
    {ForEachVertex(from,pVertex) {
        total_a_voir.insert(pVertex);
    }}
    VERBOSE_ASSERT(total_a_voir.size() > 0,TXT_NEVER_HAPPEND);
    VERBOSE_DEBUG("total_a_voir built : size = " << total_a_voir.size());

    // ** Set First Edge
    Edge firstc       = from->getFirstEdge();
    Vertex sourcefirst     = from->getEdgeSource(firstc);
    Vertex targetfirst     = from->getEdgeTarget(firstc);
    TOKEN_UNIT infirst   = from->getEdgeIn(firstc);
    TOKEN_UNIT outfirst  = from->getEdgeOut(firstc);
    alphas[firstc]       = TOKEN_FRACT(1,std::gcd(infirst,outfirst)); // 1 / gcd , the minimum value

    // ** set to view source and target, remove in to view list
    prochains_a_voir.insert(sourcefirst);
    if (targetfirst != sourcefirst)  prochains_a_voir.insert(targetfirst);
    total_a_voir.erase(sourcefirst);
    total_a_voir.erase(targetfirst);

    VERBOSE_DEBUG("First done");

    VERBOSE_DEBUG("alphas size = " << alphas.size());

    // ** Parcours des liens connexes
    while (prochains_a_voir.size() > 0) {

        // ** get next
        Vertex t = * prochains_a_voir.begin();
        prochains_a_voir.erase(t);
        VERBOSE_DEBUG("traitement de la tâche : " << from->getVertexName(t));

        // ** set ZI_REF (and check already computed alpha with ZI ref)
        VERBOSE_DEBUG("Check for ZI_REF in " << from->getVertexName(t) );
        TOKEN_FRACT ZI_REF = 0;
        {ForConnectedEdges(from,t,pEdge) {
            VERBOSE_DEBUG(" Edge ID   = " << from->getEdgeId(pEdge));
            VERBOSE_DEBUG(" Edge NAME = " << from->getEdgeName(pEdge));
            TOKEN_UNIT in   = from->getEdgeIn(pEdge);
            TOKEN_UNIT out  = from->getEdgeOut(pEdge);

            if (alphas.find(pEdge) != alphas.end())  { // channel already set : good !
                VERBOSE_DEBUG(" - is a good channel (already saw)");
                TOKEN_UNIT  V =  (from->getEdgeSource(pEdge) == t)  ? in : out ; // V * alpha = Zi
                TOKEN_FRACT A = alphas[pEdge];
                TOKEN_FRACT ZI_CUR   = (A * V);
                VERBOSE_DEBUG(" - ZI_CUR = " << ZI_CUR << "=" << A << "*" << V);
                VERBOSE_DEBUG(" - ZI_REF = " << ZI_REF);
                if (ZI_REF == TOKEN_FRACT(0)) ZI_REF = ZI_CUR;
                VERBOSE_DEBUG(" - ZI_REF = " << ZI_REF);
                if ((ZI_REF <= TOKEN_FRACT(0)) || (ZI_CUR <= TOKEN_FRACT(0))) {
                    VERBOSE_ERROR("Error in normalization process ZI_REF=" << ZI_REF << " ZI_CUR=" << ZI_CUR << " A=" << A << " V=" << V << " (A * V)=" << (A * V));
                }

                //VERBOSE_ASSERT(ZI_REF == ZI_CUR,"not consitent");
                if (ZI_REF != ZI_CUR) {
                    VERBOSE_ERROR("not consistent graph");
                    return NULL;
                }
            } else {
                VERBOSE_DEBUG(" - is skip");
            }
        }}

        // fail with not connex graph
        //assert(ZI_REF != 0);

        // ** propagate Zi
        VERBOSE_DEBUG("propagate Zi in " << from->getVertexName(t) );
        {ForConnectedEdges(from,t,pEdge) {
                 VERBOSE_DEBUG(" Edge " << from->getEdgeName(pEdge));
                 TOKEN_UNIT in   = from->getEdgeIn(pEdge);
                 TOKEN_UNIT out  = from->getEdgeOut(pEdge);

                 // ** set alpha for channel
                 if (alphas.find(pEdge) == alphas.end())  { // channel not set : we set it !
                     VERBOSE_DEBUG(" - need to be set");
                     TOKEN_UNIT  V    =  (from->getEdgeSource(pEdge) == t)  ? in : out ; // V * alpha = Zi
                     alphas[pEdge] =  ZI_REF / TOKEN_FRACT(V);
                     VERBOSE_DEBUG(" - ZI_" <<   from->getVertexName(t)  <<  " = " << ZI_REF << " Alpha = " << alphas[pEdge]);
                 } else {
                     VERBOSE_DEBUG(" - already set");
                 }
        }}


        // ** k multiply to keep integrity
        {ForConnectedEdges(from,t,pEdge) {
            TOKEN_UNIT fineGCD = from->getFineGCD(pEdge);

            TOKEN_FRACT fineW  = alphas[pEdge] * fineGCD;

            // ** move to integerization
            if (fineW.numerator() % fineW.denominator() != 0) {
                  VERBOSE_DEBUG("Computed prod/cons are not integers, check for a k multiplicator ...");

                  TOKEN_FRACT  K = TOKEN_FRACT(fineW.denominator() ,1);
                  for(std::map<Edge,TOKEN_FRACT>::iterator it = alphas.begin() ; it != alphas.end() ; it++) (*it).second = (*it).second * K;
                  VERBOSE_DEBUG("K = " << K);
                  VERBOSE_DEBUG("Zi = " << ZI_REF << " became " << (ZI_REF * K));
                  ZI_REF = ZI_REF * K;
                  if ( ZI_REF <= TOKEN_FRACT(0)) return NULL;
                  VERBOSE_ASSERT( ZI_REF > 0 , "Error in normalization process");
            }
        }}

#ifdef REENTRANCY_NORMALIZED

    // ** add reentrency integrity (on souhaite que Zi soit multiple du nombre de phase)
     TOKEN_FRACT fineW  =  ZI_REF / fromAsPhasePhasedGraph->getPhasesQuantity(t) ;
     if (fineW.numerator() % fineW.denominator() != 0) {
         VERBOSE_DEBUG("reentrancy is not integer, check for a k multiplicator ...");
         TOKEN_FRACT  K = TOKEN_FRACT(fineW.denominator() ,1);
         for(std::map<Edge,TOKEN_FRACT>::iterator it = alphas.begin() ; it != alphas.end() ; it++) (*it).second = (*it).second * K;
         VERBOSE_DEBUG("Zi = " << ZI_REF << " became " << (ZI_REF * K) << "(reentrancy)");
         ZI_REF = ZI_REF * K;
     }
#endif
        // no connexity
        //VERBOSE_ASSERT(ZI_REF != 0,TXT_NEVER_HAPPEND);

        // ** add next task to do
        {ForConnectedEdges(from,t,pEdge) {
            Vertex source     = from->getEdgeSource(pEdge);
            Vertex target     = from->getEdgeTarget(pEdge);
            if (total_a_voir.find(source) != total_a_voir.end())     prochains_a_voir.insert(source);
            if (total_a_voir.find(target) != total_a_voir.end())     prochains_a_voir.insert(target);
            total_a_voir.erase(source);
            total_a_voir.erase(target);
        }}


        // en cas d'absence de connex on chop deux nouvelles tâches
        if ((prochains_a_voir.size() <= 0) && (total_a_voir.size() > 0)) {
            {ForEachEdge(from,pEdge) {
                Vertex source     = from->getEdgeSource(pEdge);
                Vertex target     = from->getEdgeTarget(pEdge);
                 if (total_a_voir.find(source) != total_a_voir.end())
                 if (total_a_voir.find(target) != total_a_voir.end()) {
                     TOKEN_UNIT infirst   = from->getEdgeIn(pEdge);
                     TOKEN_UNIT outfirst  = from->getEdgeOut(pEdge);
                     alphas[pEdge]       = TOKEN_FRACT(1,std::gcd(infirst,outfirst)); // 1 / gcd , the minimum value

                     // ** set to view source and target, remove in to view list
                     prochains_a_voir.insert(source);
                     if (source != target)  prochains_a_voir.insert(target);
                     total_a_voir.erase(source);
                     if (source != target)  total_a_voir.erase(target);
                     break;
                 }
            }}
        }

    } // end of  while (prochains_a_voir.size() > 0)

    bool sol = true;

    if (sol)  {

           std::map<Vertex,TOKEN_UNIT> * res = new  std::map<Vertex,TOKEN_UNIT>();

           {ForEachEdge(from,pEdge) {
                    VERBOSE_DEBUG(" Edge " << from->getEdgeName(pEdge));

                    Vertex       source   = from->getEdgeSource(pEdge);
                    Vertex       target   = from->getEdgeTarget(pEdge);
                    TOKEN_UNIT     in   = from->getEdgeIn(pEdge);
                    TOKEN_UNIT     out  = from->getEdgeOut(pEdge);
                    TOKEN_FRACT alphac  = alphas[pEdge];
                    VERBOSE_ASSERT(in > 0,TXT_NEVER_HAPPEND);
                    VERBOSE_ASSERT(out > 0,TXT_NEVER_HAPPEND);
                    if (alphac  <= TOKEN_FRACT(0)) return NULL;
                    VERBOSE_ASSERT(alphac  > TOKEN_FRACT(0),TXT_NEVER_HAPPEND);
                    TOKEN_FRACT Zi      = alphac * in;
                    TOKEN_FRACT Zj      = alphac * out;
                    VERBOSE_DEBUG(" Zi = " << alphac << " * " << in << " = " << Zi);
                    VERBOSE_DEBUG(" Zj = " << alphac << " * " << out << " = " << Zj);
                    VERBOSE_ASSERT(Zi.numerator() % Zi.denominator() == 0,TXT_NEVER_HAPPEND);
                    VERBOSE_ASSERT(Zj.numerator() % Zj.denominator() == 0,TXT_NEVER_HAPPEND);
                    if (Zi.numerator() <= 0) return NULL;
                    if (Zj.numerator() <= 0) return NULL;
                    VERBOSE_ASSERT(Zi.numerator() > 0,TXT_NEVER_HAPPEND);
                    VERBOSE_ASSERT(Zj.numerator() > 0,TXT_NEVER_HAPPEND);
                    VERBOSE_ASSERT(Zi.denominator() > 0,TXT_NEVER_HAPPEND);
                    VERBOSE_ASSERT(Zj.denominator() > 0,TXT_NEVER_HAPPEND);
                    VERBOSE_ASSERT((Zi.numerator() / Zi.denominator()) > 0,TXT_NEVER_HAPPEND);
                    VERBOSE_ASSERT((Zj.numerator() / Zj.denominator()) > 0,TXT_NEVER_HAPPEND);
                    res->insert(std::pair<Vertex,TOKEN_UNIT>(source,Zi.numerator() / Zi.denominator()));
                    res->insert(std::pair<Vertex,TOKEN_UNIT>(target,Zj.numerator() / Zj.denominator()));
           }}

           //fail with alone task
           VERBOSE_ASSERT_EQUALS(res->size() , from->getVerticesCount());
           return res;

     } else {return NULL;}

}

bool algorithms::normalize (models::Dataflow *from) {

	if (from->is_normalized()) return true;

	from->set_read_only();

    bool res;

    // Generate rational normalization
    std::map<Vertex,TOKEN_UNIT> *  current =  algorithms::rationalNormalize(from);

    if (!check_validity(from,current)) {
        VERBOSE_ERROR("Normalization failed");
        return false;
    }
    // If one of both work, set normalization
    if (current) {
        VERBOSE_INFO("Normalization computation done, setting now ...");
        res = setNormalization(from,current);
    }
    else {
        VERBOSE_ERROR("Normalization failed");
        delete current;
        return false;
    }
    delete current;
    VERBOSE_INFO("Setting done.");
    if (res) from->set_normalize();
    return res;

}











