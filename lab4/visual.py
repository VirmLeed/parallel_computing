import matplotlib.pyplot as plt

class KDNode:
    def __init__(self, point, left=None, right=None):
        self.point = point
        self.left = left
        self.right = right

def read_kd_tree(file_path):
    points = []
    with open(file_path, 'r') as file:
        for line in file:
            x, y = map(float, line.split())
            points.append((x, y))
    return points

def plot_kd_tree(node, depth=0, min_x=float('-4'), max_x=float('4'), min_y=float('-4'), max_y=float('4')):
    if node is None:
        return

    # Determine the axis for the current depth
    axis = depth % 2

    # Draw the point
    plt.scatter(node.point[0], node.point[1], color='blue')

    # Draw the split line
    if axis == 0:  # Vertical line
        plt.plot([node.point[0], node.point[0]], [min_y, max_y], color=(0, 0, (10-depth*2.5)/10), linewidth=5-depth, linestyle="--")
        print([node.point[0], node.point[0]], [min_y, max_y])
        plot_kd_tree(node.left, depth + 1, min_x, node.point[0], min_y, max_y)
        plot_kd_tree(node.right, depth + 1, node.point[0], max_x, min_y, max_y)
    else:  # Horizontal line
        plt.plot([min_x, max_x], [node.point[1], node.point[1]], color=(0, depth/3, (10-depth*2.5)/10), linewidth=5-depth, linestyle="--")
        plot_kd_tree(node.left, depth + 1, min_x, max_x, min_y, node.point[1])
        plot_kd_tree(node.right, depth + 1, min_x, max_x, node.point[1], max_y)
        
def build_kd_tree(points, depth):
    if not points:
        return None

    # Select axis based on depth
    axis = depth % 2
    points.sort(key=lambda point: point[axis])
    median = len(points) // 2

    # Create a new node and construct subtrees
    return KDNode(
        point=points[median],
        left=build_kd_tree(points[:median], depth + 1),
        right=build_kd_tree(points[median + 1:], depth + 1)
    )

def main():
    points = read_kd_tree('tree.txt')

    # Build the kd-tree from the points
    root = build_kd_tree(points, 0)

    # Plot the kd-tree
    plt.figure(figsize=(8, 8))
    plot_kd_tree(root)
    plt.title('kd-tree Visualization with Splits')
    plt.xlabel('X-axis')
    plt.ylabel('Y-axis')
    plt.xlim(-4, 4)
    plt.ylim(-4, 4)
    plt.grid()
    # plt.show()
    plt.savefig("img.png")

if __name__ == "__main__":
    main()
