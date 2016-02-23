var utilsClientQ4S = {
  sum: function(list) {
        var sum = 0;
        for(var i = 0, len = list.length; i < len; i++)
        	sum += list[i];
        return sum;
  },
  isEven: function(number) {
    return(number % 2 ? false : true);
  },
  mean: function(list) {
        return list.length ? this.sum(list) / list.length : false;
  },
  median: function(list) {
    if(!list.length) return false;
    
    list.sort();
    
    if(this.isEven(list.length)) {
        return this.mean([list[list.length / 2 - 1], list[list.length / 2]]);
    } else {
        return list[Math.floor(list.length / 2)];
    }
  }
};


