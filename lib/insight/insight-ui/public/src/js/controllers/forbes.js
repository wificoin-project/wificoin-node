'use strict';

angular.module('insight.forbes').controller('RichestController',
   function($scope,$http) {
     function _bsort(x, y) {
       return (x.balance < y.balance) ? 1 : -1
     }

     $http.get('/insight/src/script/wfc_rich_list.json').success(function(data) {
        data.list.sort(_bsort);

        $scope.list = data.list.slice(0, 100);
        $scope.countTime = data.runtime;
      });

   });
