/*!

  \file testpoly2d.cc

  \brief Test for polygon2d class

*/

int main()
{
  cout << "---------- Test for poly2d ----------" << endl;
  cout<<"poly 1:"<<endl;
  pnt2d p3( 1, 1 );
  pnt2d p4( 2, 1 );
  pnt2d p5( 2, 2 );
  pnt2d p6( 1, 2 );

  print( p3 );
  print( p4 );
  print( p5 );
  print( p6 );

  vector<pnt2d> vec1, vec2;

  vec1.push_back( p3 );
  vec1.push_back( p4 );
  vec1.push_back( p5 );
  vec1.push_back( p6 );

  vec2.push_back( p6 );
  vec2.push_back( p5 );
  vec2.push_back( p4 );
  vec2.push_back( p3 );

  poly2d pol1( vec1 );
  poly2d pol2( vec2 );

  // -------------------

  cout<<"\n pol1 = poly2d( vec( p3->p4->p5->p6 ) )," << endl
      <<" pol2 = poly2d( vec( p6->p5->p4->p3 ) )," << endl;

  cout<<"\npol1.center():\n";
    print( pol1.center() );
    cout<<"pol2.center():\n";
    print( pol2.center() );

  cout<<"\npol1.is_convex(), pol2.is_convex():\n"
      << pol1.is_convex()<< "\t" << pol2.is_convex() << endl;

  cout<<"\npol1.area(), pol2.area():\n"
      << pol1.area()<< "\t" << pol2.area() << endl;

  cout<<"\npol1.mom_of_ine(), pol2.mom_of_ine():\n"
      << pol1.mom_of_ine()<< "\t" << pol2.mom_of_ine() << endl;

  cout<<"\npol1.is_CCW_oriented(), pol2.is_CCW_oriented():\n"
      << pol1.is_CCW_oriented()<< "\t" << pol2.is_CCW_oriented() << endl;

  cout<<"\npol1.is_CCW_oriented_NC(), pol2.is_CCW_oriented_NC():\n"
      << pol1.is_CCW_oriented_NC()<< "\t" << pol2.is_CCW_oriented_NC() << endl;

  cout<<"\n---- intersection ----\n";

  pnt2d p7( 1.5, 1 );
  pnt2d p8( 2.5, 1 );
  pnt2d p9( 2.5, 2 );
  pnt2d p10( 1.5, 2 );

  pnt2d p11( 2, 1 );
  pnt2d p12( 3, 1 );
  pnt2d p13( 3, 2 );
  pnt2d p14( 2, 2 );

  cout<<"poly 3:"<<endl;
  print( p7 );
  print( p8 );
  print( p9 );
  print( p10 );
  cout<<"\n\npoly 4:"<<endl;
  print( p11 );
  print( p12 );
  print( p13 );
  print( p14 );

  vector<pnt2d> vec3, vec4, res;

  vec3.push_back( p7 );
  vec3.push_back( p8 );
  vec3.push_back( p9 );
  vec3.push_back( p10 );

  vec4.push_back ( p11 );
  vec4.push_back ( p12 );
  vec4.push_back ( p13 );
  vec4.push_back ( p14 );

  poly2d pol3( vec3 );
  poly2d pol4( vec4 );

  pnt2d cen;
  double size;

  cout<<"\nintersection: poly1, poly3:\ntrue/false: "
      <<intersect(pol1, pol3,res, cen, size)
      <<"    size: "<< size <<"    center: ";
  print( cen );

  cout<<"\nintersection: poly1, poly4:\ntrue/false: "
      <<intersect(pol1, pol4,res, cen, size)
      <<"    size: "<< size <<"    center: ";
  print( cen );

  return 0;
}
