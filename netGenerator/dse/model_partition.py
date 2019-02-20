# Author Jiong He on 28 June 2018 at ADSC
#!/usr/bin/env python

from itertools import permutations


def partition(layerlist):
    """
    partition is a generator that generates all possible partitions of a given layer list in to all possible
    number (i.e., 1 to len(LayerList)) of subpartititons.
    :param layerlist: a list containing each layer information in the form of a tuple (layer index, layer name).
    :return: generate one possible partition
    """
    if len(layerlist) == 1:
        yield [layerlist]
    else:
        first = layerlist[0]
        for each_partition in partition(layerlist[1:]):
            # first choice: insert the first element into each of the subpartition's subsets
            for groupIdx, group in enumerate(each_partition, 0):
                yield each_partition[:groupIdx] + [[first] + group] + each_partition[groupIdx + 1:]
            # second choice: insert the first as an individual subset
            yield [[first]] + each_partition


def partition_to_k(layerlist, k, order=False):
    """
    partition_to_k calls partition function and filters those partitions that has k number of groups. If order is True,
    all permutations of this partition will be treated as different partition and output one by one.
    :param layerlist: a list containing each layer information in the form of a tuple (layer index, layer name).
    :param k: number of groups to partitioned into
    :param order: whether the order of groups matter
    :return: generate one possible partition
    """
    for each_partition_candidate in partition(layerlist):
        if len(each_partition_candidate) == k:
            if not order:
                yield each_partition_candidate
            else:
                for enum_item in permutations(each_partition_candidate):
                    yield enum_item


if __name__ == "__main__":
    # layer_list_1 = [(0, 'c'), (1, 'p'), (2, 'c'), (3, 'c'), (4, 'p'), (5, 'c')]
    layer_list_2 = [(0, 'c'), (1, 'p'), (2, 'c'), (3, 'c')]
    layer_list_3 = [1, 2, 3, 4]
    for idx, item in enumerate(partition_to_k(layer_list_3, 3, False), 1):
        print item