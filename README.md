# 2D Polygon Decomposition
The main objective of polygon decomposition is to extract features from 2d layer data in G-code files. This can help the slicer identify possible start points of moves and optimize the printer paths. The test application here takes in a .txt file as an input (which has a list of vertices or X,Y coordinates) and creates a list of polygon edges, where each edge is tagged with a feature ID based on different checks that are run as part of the feature detection algorithm (See PolyFeatureDetection class) 

![](/Screenshots/TestExample.png)

**Simple example of input vertices and expected feature detection**


## Background
- For the purpose of this project we assume that input set of vertices always define a simple closed polygon with no holes and full connectivity. Also these polygons cannot be self-intersecting. Attached are some examples that show vertices of input polygons and the colors indicate output features that should be idenitified by our polygon decomposition algorithm.

![](/Screenshots/PolygonTestCases.png)

**Example polygons generated from g-code vertices** 


## Current Status

Proof-of-concept implementation was first done in a QT application with the following customizable parameters :
- Line slope tolerance
- Arc radius tolerance
- Spline approximation error tolerance
- Sharp angle tolerance (maximum allowed angle between adjacent edges to be considered sufficiently smooth to be a part of the same feature).

Also following options are available for data visualiztaion :
- Scaling of graphics item using "Scale" spinbox.
- Points display ON/OFF using "Show Datamarkers" checkbox.
- Refresh display of graphics view using "Refresh" button and apply the selected tolerance checks using "Apply" button.

Following functions are implemented :
- **Line slope tolerance** checks for slope difference between subsequent edges of polygon and tags edges with same ID (starting with 99) as long as ( normalized slope differece < tolerance value ). Default value is 0.001
- **Arc Radius Tolerance** checks for center of curvature for every subsequent 3 pairs of edges and tags edges with same ID (starting with 49999) as long as calculated radius of curvature < tolerance value. Default value is 0.01
- **Spline error tolerance** List of input points is approximated by a natural cubic spline. 
If spline approximation is not possible for all points, remove points one at a time, and introduce a spline break. Now the resulting 2 sets will be approximated by 2 separate splines and so on.
Each polygon edge is finally tagged with a spline-error, which indicates the least squares error calculated from the edge to its corresponding piecewise spline approximation, and also with a feature ID (starting with 99999) that indicates the spline this edge belongs to. 
Default tolerance value is 0.05 
- **Sharp angle tolerance** checks for angle between adjacent edges and any edge that makes an angle of more than 10 degrees by default with previous edge is still considered part of the same feature, since these edges are sifficiently smooth. 
Any edge that makes an angle of less  than 10 degrees with previous edge is considered to be sharp, and is tagged with a new sharp-edge ID. (See mSharpEdgeID property in PolygonEdge object ). 
Default value of angle tolerance is 10 (see https://www.cati.com/blog/2018/04/stl-output-settings-cad-updated-2018/) 

Order of function calls is : splines, then arcs, then lines. 
- Edges that are tagged as Splines will not be run through arc/line checks
- Edges that are tagged as Arcs will NOT be run through Line checks
- Sharp angle checks are integrated into each of these individual functions if "Sharp angle tolerance" checkbox is ON. 
    - Sharp edges will be detected first, and then within each smooth-edge group, we run the spline/arc/line checks.

**Recommendations**
1. Spline approximation can be performance intensive, so for applications where ONLY the start or end point of a feature is significant, use "Sharp angle" checks ONLY instead of opting for spline curve modelling 
2. Spline tolerance checks are very sensitive to tolerance value, and so the tolerance value needs to be carefully calculated based on a factor of the printer extrusion width or smallest printable distance.
3. Above checks are currently run sequentially on the input points, so while a certain edge might be tagged with feature ID 1001, it might make more sense that it actually belongs to feature ID 1002. To improve accuracy of feature detection, run the above checks on points list in BOTH forwards and backwards direction, and then combine these  results to find all possible feature markers or end-points.
4. **Sharp Angle** checks are only effective when points are equally spaced along the polygon. If some entities are very closely spaced together, you might see a sharp angle visually,  but at the points-level, no sharp angle is detected between adjacent small edges.


## Outstanding Issues
- In the current implementation there is always a feature break at the first point in input list. But the feature can actually loop around the last point in list and might include the first few points from input list of points as well. That is, there is no guarantee that the first point in list always marks the beginning of a feature.  But our algorithm does NOT consider this special case. 
    - Use sharp-angle checks as a way to find all possible candidates for feature start points and re-arrange the input list of points to start from a good candidate point. 

### Compilation and Installation
- N/A

## Usage Instructions

![Feature detection example](/Screenshots/FeatDetect1.png)
    
**Fig 1.1 Example of feature detection using line slope tolerance checks** 


1. Drag and drop a .txt file (containing vertex coordinates) into the text box seen under "File path : ". 
2. Click "Load" to load this data into the Graphics view.
3. If the displayed polygon is too small, use the "Scale" spinbox to scale it up.
4. Use the "Show Data Markers" checkbox to turn on points display , in addition to the edges display.
5. To detect feature in this polygon, select the "Decompose Polygon" option
6. Then select all the checks that you would like to be applied as part of feature detection, such as "Line slope tolerance", "Arc radius tolerance", "Spline tolerance" and "Sharp angle tolerance". Ideally you would want to turn ON all these options with appropriate tolerances specified. If none of these options are selected, then feature detection will not be applied.
7. Once above selections are done, click "Apply". If display does not refresh, then click "Refresh". You can also use the "Scale" spinbox to force a refresh of the graphics item.
8. You will see individual features being identified represented in different colors of the edges in the display.


## Provided test cases

- Examples of line slope checks : 

    ![](/Screenshots/LineTol1.png)
    ![](/Screenshots/LineTol2.png)
   
      
- Examples of Arc/curvature checks :

    ![](/Screenshots/ArcTol1.png)
    ![](/Screenshots/ArcTol2.png)
    
    
- Examples of Spline approximation checks :
    
    ![](/Screenshots/SplineTol1.png)
    ![](/Screenshots/SplineTol2.png)    
       
   
- Examples of Sharp angle checks :
    
    ![](/Screenshots/SharpAngle1.png)
    ![](/Screenshots/SharpAngle2.png)
    
    
- Examples of combined checks (Lines/Arcs/Splines and Sharp Angles)

    ![](/Screenshots/combined1.png)
    ![](/Screenshots/combined2.png)
    
    
## Future enhancements
1. Sampling of points plays a significant role in detection of feature start and end. If possible, re-sample points in the input polygon so that they are more-or-less equally spaced from each other. Sharp angle checks will be much more accurate if points are sampled at equal intervals.
	- For example, following test case shows an example where sharp angle checks will fail to identify new features on certain parts of the polygon

	![](/Screenshots/SharpAngleFail1.png)
		
	![](/Screenshots/SharpAngleFail2.png)
	
	**Decomposition result after running sharp angle tolerance checks with angle = 10 degrees**
    
    
2. Approximate or Weak Convex decomposition of polygons is another helpful tool in identifying visually significant features. This is often used on 3d models to extract important convex sub-regions in the model. 
    - Exact Convex decomposition is NP-hard, overly strict and conservative. Approximate Convex Decomposition, on the other hand is easier to compute than an exact one. 
    - Approximate convex decomposition should be used in combination with a variety of  constrained measures to improve the quality of partition, - we recursively partition target polygon until all remaining components have concavity less than tolerance “delta” to obtain naturally visual, elegant decomposition. The recursive technique is based on decomposing target polygon at noticeable features, select all possible visible diagonals for resolving X, pick the one with highest score.
    -  You can also implement a weak convex decomposition by calculating a line-of-sight matrix based on intersection of each edge, with every other possible diagonal in the polygon. However this can be time-sensistive in case of large number of input points. The time complexity of this algorithm is atleast O(N^2*logN) . The aim is to generate weakly convex clusters : cluster in which all pairs of points are in a convex position : (1) each cluster has high convexity rank (high intra-cluster number of Line-of-sight pairs) (2) visibility between clusters is low (low inter-cluster number of Line-of-sight pairs)
    - Convexity rank of original shape is usually low but this method successfully decomposes the shape into approximately convex parts ("concavity tolerance" is used to define the convexity rankof a set of points, that is,  the portion of mutually visible pairs of points out of the total number of pairs). This method uses shape segmentation based on spectral clustering, and the affinity matrix is based on visibility.
    - Input = Affinity matrix A-nxn , Output = clustering of objects into k clusters. 
    - "Level of detail" and "Concavity tolerance" are adjustable parameters to this weak convex decomposition function. (see http://courses.cs.tau.ac.il/~dcor/articles/2013/Weak-Convex-Decomposition.pdf)
    

### External Dependencies
Use Eigen library for calculating Spline coefficients.


### Additional Resources
- N/A

### Reference libraries, docs and examples :
1. https://stackoverflow.com/questions/5247994/simple-2d-polygon-triangulation
2. http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.21.4507&rep=rep1&type=pdf
3. https://www.researchgate.net/publication/220932625_Detection_of_Shapes_in_2D_Point_Clouds_Generated_from_Images
4. http://www.math.ttu.edu/~jinsu/papers/ICPRShapeDetection.pdf
5. ttps://docs.opencv.org/3.0-beta/doc/py_tutorials/py_imgproc/py_houghlines/py_houghlines.html
6. https://matthew-brett.github.io/teaching/pca_introduction.html
7. https://cgal.geometryfactory.com/CGAL/doc/master/Shape_detection/Shape_detection_2region_growing_on_point_set_2_8cpp-example.html#_a8
8. https://www.geometrictools.com/Documentation/TriangulationByEarClipping.pdf
9. http://ranger.uta.edu/~chqding/papers/KmeansPCA1.pdf
10. https://courses.cs.washington.edu/courses/cse455/09wi/readings/fergus.pdf
11. https://www.intechopen.com/books/recent-applications-in-data-clustering/point-cloud-clustering-using-panoramic-layered-range-image
12. https://stats.stackexchange.com/questions/183236/what-is-the-relation-between-k-means-clustering-and-pca
13. http://www.mosismath.com/NaturalSplines/NaturalSplines.html
14. https://legacy.essie.ufl.edu/~kgurl/Classes/Lect3421/Fall_01/NM5_curve_f01.pdf
15. Approximate convex decomposition of polygons - By Jyh-Ming Lien, Nancy M. Amato : http://xanthippi.ceid.upatras.gr/people/psarakis/publications/Shape_Decomposition.pdf
16. New Framework for Decomposing a Polygon with Zero or More Holes  - By Zhou Hongguang and Wang Guozhao : https://benthamopen.com/contents/pdf/TOCSJ/TOCSJ-9-390.pdf
17. http://courses.cs.tau.ac.il/~dcor/articles/2013/Weak-Convex-Decomposition.pdf








