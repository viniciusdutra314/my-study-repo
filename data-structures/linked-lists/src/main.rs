#[derive(Clone)]
struct Node<T> {
    element: T,
    next_node: *mut Node<T>,
}

struct List<T> {
    num_elements: usize,
    head: *mut Node<T>,
    tail: *mut Node<T>,
}
impl<T> Default for List<T> {
    fn default() -> Self {
        Self {
            num_elements: 0,
            head: std::ptr::null_mut(),
            tail: std::ptr::null_mut(),
        }
    }
}
impl<T> Drop for List<T> {
    fn drop(&mut self) {
        let mut current_node = self.head;
        loop {
            if current_node.is_null() {
                break;
            }
            let next_node = unsafe { (*current_node).next_node };
            unsafe { std::ptr::drop_in_place(current_node) };
            current_node = next_node;
        }
    }
}

impl<T> List<T> {
    pub fn push_back(&mut self, element: T) {
        let node_box = Box::new(Node {
            element,
            next_node: std::ptr::null_mut(),
        });
        let node_raw = Box::into_raw(node_box);

        unsafe {
            if self.tail.is_null() {
                self.head = node_raw;
                self.tail = node_raw;
            } else {
                (*self.tail).next_node = node_raw;
                self.tail = node_raw;
            }
        }
        self.num_elements += 1;
    }
}
impl<T> List<T> {
    pub fn iter(&self) -> Iter<'_, T> {
        Iter {
            current: self.head,
            _marker: std::marker::PhantomData,
        }
    }
}

pub struct Iter<'a, T> {
    current: *mut Node<T>,
    _marker: std::marker::PhantomData<&'a T>,
}

impl<'a, T> Iterator for Iter<'a, T> {
    type Item = &'a T;
    fn next(&mut self) -> Option<Self::Item> {
        if self.current.is_null() {
            None
        } else {
            unsafe {
                let node = &*self.current;
                self.current = node.next_node;
                Some(&node.element)
            }
        }
    }
}

impl<'a, T> IntoIterator for &'a List<T> {
    type Item = &'a T;
    type IntoIter = Iter<'a, T>;
    fn into_iter(self) -> Self::IntoIter {
        self.iter()
    }
}

fn main() {
    let mut list = List::<i32>::default();
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    for element in &list {
        println!("{}", element);
    }
}
