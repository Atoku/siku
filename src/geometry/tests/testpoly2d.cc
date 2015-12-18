/*!

  \file testpoly2d.cc

  \brief Test for polygon2d class

*/

#include "./polygon2d.hh"

int main()
{
  cout << "---------- Test for cvpoly2d ----------" << endl;

   cout<<"poly 1:"<<endl;
   pnt2d p3( 1, 1 );
   pnt2d p4( 2, 1 );
   pnt2d p5( 2, 2 );
   pnt2d p6( 1, 2 );

   print( p3 );
   print( p4 );
   print( p5 );
   print( p6 );

   cout << "\npoly 2 is the same, but CW oriented" << endl;

   vector<pnt2d> vec1, vec2;

   vec1.push_back( p3 );
   vec1.push_back( p4 );
   vec1.push_back( p5 );
   vec1.push_back( p6 );

   vec2.push_back( p6 );
   vec2.push_back( p5 );
   vec2.push_back( p4 );
   vec2.push_back( p3 );

   cvpoly2d pol1( vec1 );
   cvpoly2d pol2( vec2 );

   // -------------------

   cout<<"\n pol1 = cvpoly2d( vec( p3->p4->p5->p6 ) )," << endl
       <<" pol2 = cvpoly2d( vec( p6->p5->p4->p3 ) )," << endl;

   cout<<"\npol1.center(), correct: (1.5, 1.5):\n";
     print( pol1.center() );
     cout<<"pol2.center(), correct: (1.5, 1.5):\n";
     print( pol2.center() );

   cout<<"\npol1.is_convex(), pol2.is_convex():\n"
       <<"correct: 1,\t1\n"
       << pol1.is_convex()<< "\t" << pol2.is_convex() << endl;

   cout<<"\npol1.area(), pol2.area():\n"
       <<"correct (for CCW): 1\t1\n"
       << pol1.area()<< "\t" << pol2.area() << endl;

   cout<<"\npol1.mom_of_ine(), pol2.mom_of_ine():\n"
       <<"correct: 1.66(6),\t166(6)\n"
       << pol1.mom_of_ine()<< "\t" << pol2.mom_of_ine() << endl;

   cout<<"\npol1.is_CCW_oriented(), pol2.is_CCW_oriented():\n"
       <<"correct: 1\t0\n"
       << pol1.is_CCW_oriented()<< "\t" << pol2.is_CCW_oriented() << endl;

   cout<<"\npol1.is_CCW_oriented_NC(), pol2.is_CCW_oriented_NC():\n"
       <<"correct: 1\t0\n"
       << pol1.is_CCW_oriented_NC()<< "\t" << pol2.is_CCW_oriented_NC() << endl;

   cout<<"\n----------------- intersection ------------------\n";

   pnt2d p7( 1.5, 1 );
   pnt2d p8( 2.5, 1 );
   pnt2d p9( 2.5, 2 );
   pnt2d p10( 1.5, 2 );

   pnt2d p11( 2, 1 );
   pnt2d p12( 3, 1 );
   pnt2d p13( 3, 2 );
   pnt2d p14( 2, 2 );

   pnt2d p15 ( 2, 2 );
   pnt2d p16 ( 3, 2 );
   pnt2d p17 ( 3, 3 );
   pnt2d p18 ( 2, 3 );

   cout<<"poly 3:"<<endl;
   print( p7 );
   print( p8 );
   print( p9 );
   print( p10 );
   cout<<"\npoly 4:"<<endl;
   print( p11 );
   print( p12 );
   print( p13 );
   print( p14 );
   cout<<"\npoly 5:"<<endl;
   print( p15 );
   print( p16 );
   print( p17 );
   print( p18 );

   vector<pnt2d> vec3, vec4, vec5, res;

   vec3.push_back( p7 );
   vec3.push_back( p8 );
   vec3.push_back( p9 );
   vec3.push_back( p10 );

   vec4.push_back ( p11 );
   vec4.push_back ( p12 );
   vec4.push_back ( p13 );
   vec4.push_back ( p14 );

   vec5.push_back ( p15 );
   vec5.push_back ( p16 );
   vec5.push_back ( p17 );
   vec5.push_back ( p18 );

   cvpoly2d pol3( vec3 );
   cvpoly2d pol4( vec4 );
   cvpoly2d pol5( vec5 );

   pnt2d cen;
   double size = 0;

   cout<<"\nintersection: poly1, poly5:\n"
       <<" correct: n = 1, s = 0, c = (2, 2)"
       <<"\n inter vertices: "
       <<intersect(pol1, pol5,res, cen, size) << endl;
   cout<<" size: "<< size <<"\n center: ";
   print( cen );

   cout<<"\nintersection: poly1, poly3:\n"
       <<" correct: n = 4, s = 0.5, c = (1.75, 1.5)"
       <<"\n inter vertices: "
       <<intersect(pol1, pol3,res, cen, size) << endl;
   cout<<" size: "<< size <<"\n center: ";
   print( cen );

   cout<<"\nintersection: poly1, poly4:\n"
       <<" correct: n = 2, s = 1, c = (2, 1.5)"
       <<"\n inter vertices: "
       <<intersect(pol1, pol4,res, cen, size) << endl;
   cout<<" size: "<< size <<"\n center: ";
   print( cen );


  return 0;
}
