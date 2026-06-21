import rclpy
from rclpy.node import Node
from demo_python_pkg.person_node import PersonNode

class WriterNode(PersonNode):
    def __init__(self, node_name: str, name: str, age: int, book: str) -> None:
        super().__init__(node_name, name, age)
        print('WriterNode 的 __init__ 函数被调用了')
        self.book = book
    def eat(self, food_name: str):
        self.get_logger().info(f'我叫{self.name},今年{self.age}岁，我现在正在吃{food_name}，我写了一本书叫{self.book}')
def main():
    rclpy.init()
    node = WriterNode('writer_node', '法外狂徒张三', 18, '论快速入狱')
    node.eat('鱼香肉丝')
    rclpy.spin(node)
